/*
 *         <ScopeGuard>  By James Taylor(ButylLee)
 *
 * ScopeGuard is a so-called Universal Resource Management
 * Class that employs RAII pattern. It provides a common way
 * of automatically releasing the resource acquired by the
 * users(programmer) via executing a provided callback when
 * leaving scope. Unlike smart pointer, ScopeGuard is not
 * taking charge of acquisition but release, referring to the
 * resources for which are too various to write control class
 * respectively, and users are supposed to acquire resource
 * first then bind it to ScopeGuard. It would operate when
 * exiting current scope by function return, exception throw, etc.
 *
 * NOTICE: You are NOT supposed to throw an exception or invoke
 * a throwing callable within ScopeGuard, if you did so, which
 * won't be prevented, the abort() will be called when sg operating.
 *
 * There are three ways to use ScopeGuard:
 *
 * (1) The recommended usage's as follows
 *
 *       -*-*-*-*-*-*-*-*-*-*-*-*-
 *       // acquire resource here
 *       ON_SCOPE_EXIT{
 *           // release statment here
 *       };
 *       -*-*-*-*-*-*-*-*-*-*-*-*-
 *     You can put statments inside { } as in normal function
 *     body, they will be execute when exiting current scope.
 *
 * (2) Using this when you want to write lambda or pass function by yourself
 *
 *       -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
 *       // acquire resource here
 *       SCOPEGUARD([&] { // release statment });
 *                  ^^^^lambda, function or executable object
 *       -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
 *
 * (3) Using function MakeScopeGuard to create named variables so that you
 *     could apply ROLLBACK pattern or modify releasing(i.e. change the
 *     lifetime of ScopeGuard).
 *
 *       -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
 *       // ROLLBACK pattern
 *       {
 *           auto sg = sg::MakeScopeGuard([&] { // release statment });
 *                                        ^^^^lambda, function or executable object
 *
 *           // do something that may fail and throw exception
 *           sg.dismiss(); // if succeed, don't rollback
 *       }
 *
 *       // prolong or shorten the lifetime(barely use)
 *       auto prolong()
 *       {
 *           // acquire resource here
 *           auto sg = sg::MakeScopeGuard([&] { // release statment });
 *           return sg;
 *       }
 *       void shorten()
 *       {
 *           // acquire resource here
 *           auto sg = sg::MakeScopeGuard([&] { // release statment });
 *           foo(std::move(sg));
 *       }
 *       -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
 */

#ifndef SCOPEGUARD_H_
#define SCOPEGUARD_H_

#define SG_USING_TEMPLATE 1

#define SG_BEGIN namespace sg {
#define SG_END   }
#define SG_LINE_NAME_CAT(name,line) name##line
#define SG_LINE_NAME(name,line) SG_LINE_NAME_CAT(name,line)


#define ON_SCOPE_EXIT \
		auto SG_LINE_NAME(OnScopeExit_Block_,__LINE__) = \
		sg::detail::eOnScopeExit() + [&]() noexcept ->void
#define SCOPEGUARD(callback) \
		auto SG_LINE_NAME(SCOPEGUARD_,__LINE__) = sg::MakeScopeGuard(callback)

#if SG_USING_TEMPLATE
#include <utility>
#include <type_traits>

SG_BEGIN
namespace detail {
	// Determining whether a type is a proper callback that ScopeGuard need
	template<typename T>
	constexpr bool is_proper_callback_v
		= std::is_same<void, decltype(std::declval<T>()())>::value; // std::is_same_v is not defined until C++17

	template<typename TCallback, typename =
		std::enable_if_t<is_proper_callback_v<TCallback>>>
	class ScopeGuard;

	/* --- The helper functions provided multiple usages --- */
	enum class eOnScopeExit {}; // dummy

	template<typename TCallback>
	constexpr ScopeGuard<TCallback> operator+(eOnScopeExit, TCallback&& callback)
	{
		return ScopeGuard<TCallback>(std::forward<TCallback>(callback));
	}

	template<typename TCallback>
	constexpr ScopeGuard<TCallback> MakeScopeGuard(TCallback&& callback)
	{
		return ScopeGuard<TCallback>(std::forward<TCallback>(callback));
	}
	/* ----------------------------------------------------- */

	template<typename TCallback>
	class ScopeGuard<TCallback> final
	{
		using Callback = std::decay_t<TCallback>;

		friend constexpr ScopeGuard<TCallback> operator+<TCallback>(eOnScopeExit, TCallback&&);
		friend constexpr ScopeGuard<TCallback> MakeScopeGuard<TCallback>(TCallback&&);
	private:
		explicit ScopeGuard(Callback callback)
			: m_callback(std::move(callback))
		{}
	public:
		~ScopeGuard() noexcept
		{
			if (m_active)
				m_callback();
		}

		ScopeGuard(ScopeGuard&& other)
			noexcept(std::is_nothrow_move_constructible<Callback>::value) // std::is_nothrow_move_constructible_v is not defined until C++17
			: m_callback(std::move(other.m_callback))
			, m_active(std::move(other.m_active))
		{
			other.dismiss();
		}
		/* No move assign function */

		void dismiss() noexcept
		{
			m_active = false;
		}

		ScopeGuard(const ScopeGuard&) = delete;
		ScopeGuard& operator=(const ScopeGuard&) = delete;
	private:
		Callback m_callback;
		bool m_active = true;
	};
} // namespace detail

using detail::MakeScopeGuard;
SG_END
#else // ^^^ SG_USING_TEMPLATE / !SG_USING_TEMPLATE vvv
#include <functional>

SG_BEGIN
namespace detail{
	enum class eOnScopeExit {}; // dummy

	// A simple and easy-understanding implement but take up more size
	class ScopeGuard final
	{
		friend ScopeGuard operator+(eOnScopeExit, std::function<void()>);
		friend ScopeGuard MakeScopeGuard(std::function<void()>);
	private:
		explicit ScopeGuard(std::function<void()> callback)
			: m_callback(callback)
		{}
	public:
		~ScopeGuard() noexcept
		{
			if (m_active)
				m_callback();
		}

		ScopeGuard(ScopeGuard&& other)
			noexcept(std::is_nothrow_move_constructible<std::function<void()>>::value) // std::is_nothrow_move_constructible_v is not defined until C++17
			: m_callback(std::move(other.m_callback))
			, m_active(std::move(other.m_active))
		{
			other.dismiss();
		}
		/* No move assign function */

		void dismiss() noexcept
		{
			m_active = false;
		}

		ScopeGuard(const ScopeGuard&) = delete;
		ScopeGuard& operator=(const ScopeGuard&) = delete;
	private:
		std::function<void()> m_callback;
		bool m_active = true;
	};

	/* --- The helper functions provided multiple usages --- */
	inline ScopeGuard operator+(eOnScopeExit, std::function<void()> callback)
	{
		return ScopeGuard(callback);
	}

	inline ScopeGuard MakeScopeGuard(std::function<void()> callback)
	{
		return ScopeGuard(callback);
	}
	/* ----------------------------------------------------- */
} // namespace detail

using detail::MakeScopeGuard;
SG_END
#endif // ^^^ !SG_USING_TEMPLATE ^^^

#undef SG_USING_TEMPLATE
#undef SG_BEGIN
#undef SG_END

#endif // SCOPEGUARD_H_
