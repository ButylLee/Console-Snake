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

template<typename Base, typename = std::void_t<typename Base::Tag>>
constexpr bool CheckCorrectEnumName = false;
template<typename Base>
constexpr bool CheckCorrectEnumName<Base, void> = true;

template<typename EnumName, typename ValueType = int, typename NameType = std::wstring>
class Enum :public EnumName
{
	static_assert(CheckCorrectEnumName<EnumName>, "Incorrect EnumName type");

	using base_type = std::remove_cv_t<ValueType>;
	using pair_type = std::pair<std::add_const_t<NameType>, base_type>;
	using custom_pair_type = std::pair<std::add_const_t<NameType>, std::optional<base_type>>;
	using list_type = std::vector<pair_type>;//std::array?

	using Tag = typename EnumName::Tag;
	using CustomTag = typename EnumName::CustomTag;

public:
	using value_type = base_type;//remove?
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
		if (cur_val == CustomTag::Custom && !enum_custom.second)
		{
			defaultValue();
		}
		else if (cur_val == enum_list.size() - 1)
		{
			if (enum_custom.second)
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
		return cur_val == CustomTag::Custom ? enum_custom.first : enum_list[cur_val].first;
	}
	constexpr base_type Value() const noexcept//value_type?
	{
		return static_cast<base_type>(*this);
	}
	constexpr operator base_type() const noexcept//value_type?
	{
		// back to default value when clearCustomValue() called and happens to be custom value
		if (cur_val == CustomTag::Custom)
		{
			if (!enum_custom.second)
				defaultValue();
			return *enum_custom.second;
		}
		return enum_list[cur_val].second;
	}
	constexpr base_type convertFrom(base_type val) noexcept//value_type?
	{
		for (size_t i = 0; i < enum_list.size(); i++)
		{
			if (val == enum_list[i].second)
			{
				cur_val = i;
				return val;
			}
		}
		setCustomValue(val);
		cur_val = CustomTag::Custom;
		return val;
	}
	static std::add_const_t<NameType> getNameFrom(base_type val) noexcept//value_type?
	{
		for (size_t i = 0; i < enum_list.size(); i++)
		{
			if (val == enum_list[i].second)
				return enum_list[i].first;
		}
		return enum_custom.first;
	}
	constexpr static void setCustomValue(base_type custom)//value_type?
	{
		enum_custom.second = std::move(custom);
	}
	constexpr static void clearCustomValue() noexcept
	{
		enum_custom.second = std::nullopt;
	}

private:
	static list_type enum_list;
	static custom_pair_type enum_custom;
	static const size_t default_shift;
	mutable size_t cur_val = default_shift;
};

#define ENUM_DEL(name) \
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