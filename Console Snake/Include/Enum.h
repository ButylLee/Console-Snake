#pragma once
#ifndef SNAKE_ENUM_HEADER_
#define SNAKE_ENUM_HEADER_

/*
 * Enum acts like built-in enum but only store value that pre-defined,
 * and could have not only integral underlying type. It has pre-defined
 * values defined and default value specified in compile-time, only one
 * custom value modified in runtime. The custom value can be null. A
 * Enum object's value can switch circularly in these defined values. It
 * can also store string_view for reflection.
 *
 * To define a Enum type like built-in enum:
 *     enum class Size :int{
 *         S = 10,
 *         M = 20,
 *         L = 30
 *     };
 *             ||
 *             vv
 *     ENUM_DEL(Size)
 *         S, M, L
 *     ENUM_DEF(Size, short) // type "short" is optional, default type is int
 *        { L"S",10 },
 *        { L"M",20 },
 *        { L"L",30 }
 *     ENUM_END
 *     ENUM_DEFAULT(Size, M)
 *
 * To use a Enum:
 *     Size s = Size::L;
 *     s.nextValue();
 *     s.defaultValue();
 *     s::setCustomValue(50);
 *     s = Size::CustomValue;
 */

#include <utility>
#include <vector>
#include <optional>
#include <type_traits>
#include <string>
#include <string_view>

template<typename Base, typename = std::void_t<typename Base::Tag>>
constexpr bool CheckCorrectEnumName = false;
template<typename Base>
constexpr bool CheckCorrectEnumName<Base, void> = true;

template<typename Name, typename TBase = int>
class Enum :public Name
{
	static_assert(CheckCorrectEnumName<Name>, "incorrect Enum Name type");
	using base_type = std::remove_cv_t<TBase>;
	using pair_type = std::pair<const std::wstring, std::optional<base_type>>;
	using list_type = std::vector<pair_type>;
	using Tag = typename Name::Tag;
public:
	using value_type = base_type;
	Enum() = default;
	// Tag literals could implicitly cast to Enum
	constexpr Enum(Tag tag) noexcept
	{
		cur_val = static_cast<size_t>(tag);
	}
public:
	constexpr Enum& nextValue() noexcept
	{
		if (cur_val == Tag::Custom && !enum_list[Tag::Custom].second)
			defaultVal();
		else if (cur_val + 1 == Tag::Custom && !enum_list[Tag::Custom].second)
			cur_val = 0;
		else
			nextVal();
		return *this;
	}
	constexpr Enum& defaultValue() noexcept
	{
		defaultVal();
		return *this;
	}
	constexpr std::wstring_view Name() const noexcept
	{
		return enum_list[cur_val].first;
	}
	constexpr base_type Value() const noexcept
	{
		return static_cast<base_type>(*this);
	}
	constexpr operator base_type() const noexcept
	{
		if (!enum_list[cur_val].second)
			defaultVal();
		return enum_list[cur_val].second.operator*();
	}
	constexpr base_type convertFrom(base_type val) noexcept
	{
		size_t i = 0;
		for (; i < enum_list.size() - 1; i++)
		{
			if (val == enum_list[i].second.operator*())
			{
				cur_val = i;
				return val;
			}
		}
		setCustomValue(val);
		cur_val = i;
		return val;
	}
	constexpr static void setCustomValue(base_type custom)
	{
		enum_list.back().second = std::move(custom);
	}
	constexpr static void clearCustomValue() noexcept
	{
		enum_list.back().second = std::nullopt;
	}
private:
	constexpr void nextVal() noexcept
	{
		if (cur_val + 1 == enum_list.size())
			cur_val = 0;
		else
			cur_val++;
	}
	constexpr void defaultVal() const noexcept
	{
		cur_val = default_shift;
	}
private:
	static list_type enum_list;
	static const size_t default_shift;
	mutable size_t cur_val = default_shift;
};

#define ENUM_DEL(name) \
struct _tag##name { \
	enum Tag :size_t {

#define ENUM_DEF(name, ...) \
		, Custom \
	}; \
}; \
using name = Enum<_tag##name, __VA_ARGS__>; \
inline name::list_type name::enum_list = {

#define ENUM_END \
	, { L"Custom",{} } \
};

#define ENUM_DEFAULT(name, default_value) \
inline const size_t name::default_shift = name::default_value;

#endif // SNAKE_ENUM_HEADER_