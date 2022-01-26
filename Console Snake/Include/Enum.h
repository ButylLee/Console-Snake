#pragma once
#ifndef SNAKE_ENUM_HEADER_
#define SNAKE_ENUM_HEADER_

/*
 *                             Enum & CustomEnum
 * Enum and CustomEnum acts like built-in enum but only store pre-defined
 * key-value, and could have not only integral underlying type. It has
 * pre-defined key-values defined and default key-value specified in
 * compile-time. CustomEnum could only have one custom value modified in
 * runtime, and the custom key-value can be null. A Enum object's value
 * can switch circularly in these defined values. It can also store string
 * for reflection.
 *
 * To define an Enum or CustomEnum type like built-in enum:
 *     enum class Size :int{
 *         S = 10,
 *         M = 20,
 *         L = 30
 *     };
 *             ||
 * transfrom it like below:
 *             ||
 *             vv
 *     struct SizeInfo {
 *         enum Tag {
 *             S, M, L,
 *             DefaultValue = S
 *         };
 *     };
 *     using Size = CustomEnum<SizeInfo, short, std::string>; // last two args are optional
 *     ENUM_DEFINE(Size)
 *     {
 *        { 10, L"S" },
 *        { 20, L"M" },
 *        { 30, L"L" }
 *     };
 *     ENUM_CUSTOM(Size)
 *     {
 *         {}, L"Custom"
 *     };
 *
 * Note: The enum "Tag" and "DefaultValue" value must be defined.
 *       The macro "ENUM_CUSTOM" is optional if using Enum<...>.
 *
 * To use an Enum:
 *     Size size = Size::L;
 *     size.setNextValue();
 *     size.setDefaultValue();
 *     size.convertFrom(20);
 *
 *     auto name = size.Name();
 *     auto value1 = size.Value();
 *     short value2 = size;
 *
 *     size::setCustomValue(50);
 *     size = Size::Custom;
 *
 * To inquire key-value:
 *     auto nameOfS = Size::getNameFrom(10);
 *     auto valueOfL = Size::getValueFrom(Size::L);
 */

#include <utility>
#include <vector>
#include <optional>
#include <type_traits>
#include <string>

namespace detail {
	template<typename, typename = void, typename = void>
	constexpr bool CheckCorrectEnumInfo = false;
	template<typename Base>
	constexpr bool CheckCorrectEnumInfo<
		Base,
		std::void_t<typename Base::Tag>,
		std::void_t<decltype(Base::DefaultValue)>> = true;
}

// ----------------- Base Class EnumBase ----------------
template<typename EnumType, typename TypeInfo>
class EnumBase
{
	using Tag = typename TypeInfo::Tag;
	using NameType = typename TypeInfo::NameType;
	using ValueType = typename TypeInfo::ValueType;

public:
	EnumBase() = default;
	// Tag literals could implicitly cast to CustomEnum
	constexpr EnumBase(Tag tag) noexcept
	{
		current_value_index = static_cast<size_t>(tag);
	}

public:
	constexpr const EnumType& setNextValue() noexcept
	{
		return static_cast<EnumType*>(this)->setNextValue();
	}
	constexpr NameType Name() const noexcept
	{
		return static_cast<const EnumType*>(this)->Name();
	}
	constexpr operator ValueType() const noexcept
	{
		return static_cast<const EnumType*>(this)->operator ValueType();
	}
	constexpr ValueType convertFrom(ValueType val) noexcept
	{
		return static_cast<EnumType*>(this)->convertFrom(val);
	}

	constexpr const EnumType& setDefaultValue() noexcept
	{
		current_value_index = TypeInfo::DefaultValue;
		return static_cast<const EnumType&>(*this);
	}
	constexpr ValueType Value() const noexcept
	{
		return static_cast<ValueType>(*this); // invoke operator value_type
	}

public:
	static constexpr NameType getNameFrom(ValueType val) noexcept
	{
		return EnumType::getNameFrom(val);
	}
	static constexpr ValueType getValueFrom(const EnumBase& tag_or_var) noexcept
	{
		return tag_or_var;
	}

public:
	friend constexpr bool operator==(const EnumBase& lhs, const EnumBase& rhs) noexcept
	{
		return lhs.current_value_index == rhs.current_value_index;
	}

protected:
	std::make_signed_t<size_t> current_value_index = TypeInfo::DefaultValue;
};

// ---------------- Helper Class EnumInfo ---------------
template<typename EnumInfoT, typename ValueT, typename NameT>
struct EnumInfo :EnumInfoT // Derived for enum items
{
	static_assert(detail::CheckCorrectEnumInfo<EnumInfoT>, "Incorrect EnumInfo type.");
	using ValueType = std::remove_cv_t<ValueT>;
	using NameType = NameT;
	using Tag = typename EnumInfoT::Tag;
};

// ------------------- Main Class Enum ------------------
template<typename EnumInfoT, typename ValueT = int, typename NameT = std::wstring>
class Enum :
	public EnumInfo<EnumInfoT, ValueT, NameT>,
	public EnumBase<Enum<EnumInfoT, ValueT, NameT>, EnumInfo<EnumInfoT, ValueT, NameT>>
{
	using Base = EnumBase<Enum<EnumInfoT, ValueT, NameT>, EnumInfo<EnumInfoT, ValueT, NameT>>;
	using Info = EnumInfo<EnumInfoT, ValueT, NameT>;
public:
	using Tag = typename Info::Tag;
	using ValueType = typename Info::ValueType;
	using NameType = typename Info::NameType;
private:
	using pair_type = std::pair<ValueType, std::add_const_t<NameType>>;
	using list_type = std::vector<pair_type>;

public:
	Enum() = default;
	constexpr Enum(Tag tag) noexcept
		:Base(tag)
	{}

public:
	constexpr const Enum& setNextValue() noexcept
	{
		if (this->current_value_index == enum_list.size() - 1)
		{
			this->current_value_index = 0;
		}
		else
		{
			this->current_value_index++;
		}
		return *this;
	}
	constexpr NameType Name() const noexcept
	{
		return enum_list[this->current_value_index].second;
	}
	constexpr operator ValueType() const noexcept
	{
		return enum_list[this->current_value_index].first;
	}
	constexpr ValueType convertFrom(ValueType val) noexcept
	{
		for (size_t i = 0; i < enum_list.size(); i++)
		{
			if (val == enum_list[i].first)
			{
				this->current_value_index = i;
				return val;
			}
		}
		this->setDefaultValue();
		return *this;
	}

public:
	static constexpr NameType getNameFrom(ValueType val) noexcept
	{
		for (size_t i = 0; i < enum_list.size(); i++)
		{
			if (val == enum_list[i].first)
				return enum_list[i].second;
		}
		return {};
	}

private:
	static list_type enum_list;
};

// ---------------- Main Class CustomEnum ---------------
struct CustomInfo
{
	enum CustomTag :int {
		Custom = -1
	};
};

template<typename EnumInfoT, typename ValueT = int, typename NameT = std::wstring>
class CustomEnum :
	public EnumInfo<EnumInfoT, ValueT, NameT>,
	public CustomInfo,
	public EnumBase<CustomEnum<EnumInfoT, ValueT, NameT>, EnumInfo<EnumInfoT, ValueT, NameT>>
{
	using Base = EnumBase<CustomEnum<EnumInfoT, ValueT, NameT>, EnumInfo<EnumInfoT, ValueT, NameT>>;
	using Info = EnumInfo<EnumInfoT, ValueT, NameT>;
public:
	using Tag = typename Info::Tag;
	using ValueType = typename Info::ValueType;
	using NameType = typename Info::NameType;
private:
	using pair_type = std::pair<ValueType, std::add_const_t<NameType>>;
	using custom_pair_type = std::pair<std::optional<ValueType>, std::add_const_t<NameType>>;
	using list_type = std::vector<pair_type>;

public:
	CustomEnum() = default;
	constexpr CustomEnum(Tag tag) noexcept
		:Base(tag)
	{}
	constexpr CustomEnum(CustomTag tag) noexcept
		:Base(static_cast<Tag>(tag))
	{}

public:
	constexpr const CustomEnum& setNextValue() noexcept
	{
		if (this->current_value_index == CustomTag::Custom && !enum_custom.first)
		{
			setDefaultValue_force();
		}
		else if (this->current_value_index == enum_list.size() - 1)
		{
			if (enum_custom.first)
				this->current_value_index = CustomTag::Custom;
			else
				this->current_value_index = 0;
		}
		else
		{
			this->current_value_index++;
		}
		return *this;
	}
	constexpr NameType Name() const noexcept
	{
		if (this->current_value_index == CustomTag::Custom)
		{
			if (enum_custom.first)
				return enum_custom.second;
			else
				setDefaultValue_force();
		}
		return enum_list[this->current_value_index].second;
	}
	constexpr operator ValueType() const noexcept
	{
		if (this->current_value_index == CustomTag::Custom)
		{
			if (enum_custom.first)
				return *enum_custom.first;
			else
				setDefaultValue_force();
		}
		return enum_list[this->current_value_index].first;
	}
	constexpr ValueType convertFrom(ValueType val) noexcept
	{
		for (size_t i = 0; i < enum_list.size(); i++)
		{
			if (val == enum_list[i].first)
			{
				this->current_value_index = i;
				return val;
			}
		}
		setCustomValue(val);
		this->current_value_index = CustomTag::Custom;
		return val;
	}

public:
	static constexpr NameType getNameFrom(ValueType val) noexcept
	{
		for (size_t i = 0; i < enum_list.size(); i++)
		{
			if (val == enum_list[i].first)
				return enum_list[i].second;
		}
		if (val == enum_custom.first)
			return enum_custom.second;
		return {};
	}

	static constexpr void setCustomValue(ValueType custom)
	{
		enum_custom.first = std::move(custom);
	}
	static constexpr std::optional<ValueType> getCustomValue() noexcept
	{
		return enum_custom.first;
	}
	static constexpr void clearCustomValue() noexcept
	{
		enum_custom.first = std::nullopt;
	}

private:
	// back to default value when clearCustomValue() called and happens to be custom value
	constexpr void setDefaultValue_force() const noexcept
	{
		const_cast<CustomEnum*>(this)->setDefaultValue();
	}

private:
	static list_type enum_list;
	static custom_pair_type enum_custom;
};

#define ENUM_DEFINE(name) \
inline name::list_type name::enum_list

#define ENUM_CUSTOM(name) \
inline name::custom_pair_type name::enum_custom

#endif // SNAKE_ENUM_HEADER_