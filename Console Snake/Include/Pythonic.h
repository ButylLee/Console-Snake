#pragma once
#ifndef PYTHONIC_HEADER_
#define PYTHONIC_HEADER_

#include <type_traits>
#include <concepts>
#include <cassert>
#include <utility>
#include <initializer_list>
#include <iterator>

template<std::integral T>
class range
{
public:
	class range_iterator
	{
	public:
		constexpr range_iterator(T pos, T step) noexcept
			:pos_(pos), step_(step)
		{}
		constexpr T operator*() const noexcept { return pos_; }
		constexpr T operator++() noexcept { return pos_ += step_; }
		constexpr T operator++(int) noexcept { return (pos_ += step_) - step_; }
		constexpr bool operator==(const range_iterator& rhs) const noexcept
		{
			return this->pos_ == rhs.pos_;
		}
		constexpr bool operator!=(const range_iterator& rhs) const noexcept
		{
			return !(*this == rhs);
		}
	private:
		T pos_;
		T step_;
	};

	template<std::integral V>
	constexpr range(V end) noexcept
		:end_(static_cast<T>(end))
	{
		check();
	}
	template<std::integral U, std::integral V>
	constexpr range(U begin, V end) noexcept
		:begin_(static_cast<T>(begin)), end_(static_cast<T>(end))
	{
		check();
	}
	template<std::integral U, std::integral V, std::integral W>
	constexpr range(U begin, V end, W step) noexcept
		:begin_(static_cast<T>(begin)), end_(static_cast<T>(end)), step_(static_cast<T>(step))
	{
		check();
	}
	constexpr auto begin() const noexcept
	{
		return range_iterator(begin_, step_);
	}
	constexpr auto end() const noexcept
	{
		return range_iterator(end_, step_);
	}

private:
	constexpr void check() noexcept
	{
		if (std::is_constant_evaluated())
		{
			if (step_ > T{ 0 } && begin_ > end_ ||
				step_ < T{ 0 } && begin_ < end_ || step_ == T{ 0 })
				begin_ = end_;
		}
		else
		{
			assert((step_ > T{ 0 } && begin_ < end_ ||
					step_ < T{ 0 } && begin_ > end_) && step_ != T{ 0 });
		}
	}
	// [begin, end)
	T begin_ = T{ 0 };
	T end_;
	T step_ = T{ 1 };
};

template<std::integral V>
range(V) -> range<V>;
template<std::integral U, std::integral V>
range(U, V) -> range<V>;
template<std::integral U, std::integral V, std::integral W>
range(U, V, W) -> range<V>;

template<typename Container>
class enumerate
{
public:
	class enumerate_iterator
	{
	public:
		using Iter = decltype(std::begin(std::declval<Container>()));

		template<typename V>
		struct item_pair
		{
			template<typename T>
			constexpr item_pair(size_t index, T&& value) noexcept
				:index_(index), value_(std::forward<T>(value))
			{}
			size_t index_;
			V value_;
		};
		template<typename V>
		item_pair(size_t, V&&) -> item_pair<V>;

	public:
		constexpr enumerate_iterator(size_t index, Iter iter) noexcept
			:index_(index), iter_(iter)
		{}
		constexpr auto operator*() const noexcept
		{
			return item_pair(index_, *iter_);
		}
		constexpr enumerate_iterator& operator++() noexcept
		{
			++index_;
			++iter_;
			return *this;
		}
		constexpr enumerate_iterator operator++(int) noexcept
		{
			enumerate_iterator tmp = *this;
			this->operator++();
			return tmp;
		}
		constexpr bool operator==(const enumerate_iterator& rhs) const noexcept
		{
			return this->iter_ == rhs.iter_;
		}
		constexpr bool operator!=(const enumerate_iterator& rhs) const noexcept
		{
			return !(*this == rhs);
		}

	private:
		size_t index_ = 0;
		Iter iter_;
	};

public:
	template<typename T>
	constexpr enumerate(T&& container) noexcept
		:container_(std::forward<T>(container))
	{}
	template<typename T>
	constexpr enumerate(std::initializer_list<T> container) noexcept
		:container_(container)
	{}
	constexpr auto begin() noexcept
	{
		return enumerate_iterator(0, container_.begin());
	}
	constexpr auto begin() const noexcept
	{
		return enumerate_iterator(0, container_.begin());
	}
	constexpr auto end() noexcept
	{
		return enumerate_iterator(container_.size(), container_.end());
	}
	constexpr auto end() const noexcept
	{
		return enumerate_iterator(container_.size(), container_.end());
	}

private:
	Container container_;
};

template<typename T>
enumerate(std::initializer_list<T>) -> enumerate<std::initializer_list<T>>;
template<typename T>
enumerate(T&&) -> enumerate<T>;

#endif // PYTHONIC_HEADER_