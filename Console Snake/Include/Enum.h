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
 *     ENUM_DECL(Size)
 *     {
 *         S, M, L
 *     }
 *     ENUM_DEF(Size, short, std::wstring) // last two args are optional
 *     {
 *        { 10, L"S" },
 *        { 20, L"M" },
 *        { 30, L"L" }
 *     };
 *     ENUM_CUSTOM(Size, {}, L"Custom");
 *     ENUM_DEFAULT(Size, M);
 *
 * Note: ALL four macros must be used.
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

template<typename Base,
		 typename = std::void_t<typename Base::Tag>,
		 typename = std::void_t<typename Base::CustomTag>>
constexpr bool CheckCorrectEnumName = false;
template<typename Base>
constexpr bool CheckCorrectEnumName<Base, void, void> = true;

template<typename EnumName, typename ValueType = int, typename NameType = std::wstring>
class Enum :public EnumName
{
	static_assert(CheckCorrectEnumName<EnumName>, "Incorrect EnumName type");

	using base_type = std::remove_cv_t<ValueType>;
	using pair_type = std::pair<base_type, std::add_const_t<NameType>>;
	using custom_pair_type = std::pair<std::optional<base_type>, std::add_const_t<NameType>>;
	using list_type = std::vector<pair_type>;

	using Tag = typename EnumName::Tag;
	using CustomTag = typename EnumName::CustomTag;

public:
	using value_type = base_type;
	Enum() = default;
	// Tag literals could implicitly cast to Enum
	constexpr Enum(Tag tag) noexcept
	{
		cur_val = static_cast<size_t>(tag);
	}
	constexpr Enum(CustomTag tag) noexcept
	{
		cur_val = static_cast<size_t>(tag);
	}

public:
	constexpr const Enum& nextValue() noexcept
	{
		// back to default value when clearCustomValue() called and happens to be custom value
		if (cur_val == CustomTag::Custom && !enum_custom.first)
		{
			defaultValue();
		}
		else if (cur_val == enum_list.size() - 1)
		{
			if (enum_custom.first)
				cur_val = CustomTag::Custom;
			else
				cur_val = 0;
		}
		else
		{
			++cur_val;
		}
		return *this;
	}
	constexpr const Enum& defaultValue() const noexcept
	{
		cur_val = default_shift;
		return *this;
	}
	constexpr std::add_const_t<NameType> Name() const noexcept
	{
		// back to default value when clearCustomValue() called and happens to be custom value
		if (cur_val == CustomTag::Custom)
		{
			if (enum_custom.first)
				return enum_custom.second;
			else
				defaultValue();
		}
		return enum_list[cur_val].second;
	}
	constexpr value_type Value() const noexcept
	{
		return static_cast<value_type>(*this);
	}
	constexpr operator value_type() const noexcept
	{
		// back to default value when clearCustomValue() called and happens to be custom value
		if (cur_val == CustomTag::Custom)
		{
			if (enum_custom.first)
				return *enum_custom.first;
			else
				defaultValue();
		}
		return enum_list[cur_val].first;
	}
	constexpr value_type convertFrom(value_type val) noexcept
	{
		for (size_t i = 0; i < enum_list.size(); i++)
		{
			if (val == enum_list[i].first)
			{
				cur_val = i;
				return val;
			}
		}
		setCustomValue(val);
		cur_val = CustomTag::Custom;
		return val;
	}

public:
	constexpr static std::add_const_t<NameType> getNameFrom(value_type val) noexcept
	{
		for (size_t i = 0; i < enum_list.size(); i++)
		{
			if (val == enum_list[i].first)
				return enum_list[i].second;
		}
		return enum_custom.second;
	}
	constexpr static std::add_const_t<value_type> getValue(const Enum& tag_or_var) noexcept
	{
		return tag_or_var;
	}
	constexpr static void setCustomValue(value_type custom)
	{
		enum_custom.first = std::move(custom);
	}
	constexpr static std::optional<value_type> getCustomValue() noexcept
	{
		return enum_custom.first;
	}
	constexpr static void clearCustomValue() noexcept
	{
		enum_custom.first = std::nullopt;
	}

public:
	friend constexpr bool operator==(const Enum& lhs, const Enum& rhs) noexcept
	{
		return lhs.cur_val == rhs.cur_val;
	}

private:
	static list_type enum_list;
	static custom_pair_type enum_custom;
	static const size_t default_shift;
	mutable size_t cur_val = default_shift;
};

#define ENUM_DECL(name) \
struct name##_tag { \
	enum Tag :int

#define ENUM_DEF(name, ...) \
	; \
	enum CustomTag :int { \
		Custom = -1 \
	}; \
}; \
using name = Enum<name##_tag, __VA_ARGS__>; \
inline name::list_type name::enum_list =

#define ENUM_CUSTOM(name, ...) \
inline name::custom_pair_type name::enum_custom = { __VA_ARGS__ }

#define ENUM_DEFAULT(name, default_value) \
inline const size_t name::default_shift = name::default_value

#endif // SNAKE_ENUM_HEADER_