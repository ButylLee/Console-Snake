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
 *     s.setNextValue();
 *     s.setDefaultValue();
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
		 typename = std::void_t<decltype(Base::DefaultValue)>>
constexpr bool CheckCorrectEnumInfo = false;
template<typename Base>
constexpr bool CheckCorrectEnumInfo<Base, void, void> = true;

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
	constexpr const EnumType& setDefaultValue() const noexcept
	{
		current_value_index = TypeInfo::DefaultValue;
		return static_cast<const EnumType&>(*this);
	}
	constexpr std::add_const_t<NameType> Name() const noexcept
	{
		return static_cast<const EnumType*>(this)->Name();
	}
	constexpr ValueType Value() const noexcept
	{
		return static_cast<ValueType>(*this); // invoke operator value_type
	}
	constexpr operator ValueType() const noexcept
	{
		return static_cast<const EnumType*>(this)->operator ValueType();
	}
	constexpr ValueType convertFrom(ValueType val) noexcept
	{
		return static_cast<EnumType*>(this)->convertFrom(val);
	}

public:
	static constexpr std::add_const_t<NameType> getNameFrom(ValueType val) noexcept
	{
		return EnumType::getNameFrom(val);
	}
	static constexpr std::add_const_t<ValueType> getValueFrom(const EnumBase* tag_or_var) noexcept
	{
		return tag_or_var;
	}

public:
	friend constexpr bool operator==(const EnumBase& lhs, const EnumBase& rhs) noexcept
	{
		return lhs.current_value_index == rhs.current_value_index;
	}

protected:
	mutable std::make_signed_t<size_t> current_value_index = TypeInfo::DefaultValue;
};

template<typename EnumInfoT, typename ValueT, typename NameT>
struct EnumInfo :EnumInfoT
{
	static_assert(CheckCorrectEnumInfo<EnumInfoT>, "Incorrect EnumInfo type.");
	using ValueType = std::remove_cv_t<ValueT>;
	using NameType = NameT;
	using Tag = typename EnumInfoT::Tag;
};

template<typename EnumInfoT, typename ValueT=int,typename NameT=std::wstring>
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
	static constexpr ValueType getValueFrom(const Enum& tag_or_var) noexcept
	{
		return tag_or_var;
	}

public:
	friend constexpr bool operator==(const Enum& lhs, const Enum& rhs) noexcept
	{
		return lhs.current_value_index == rhs.current_value_index;
	}

private:
	static list_type enum_list;
};

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
		// back to default value when clearCustomValue() called and happens to be custom value
		if (this->current_value_index == CustomTag::Custom && !enum_custom.first)
		{
			this->setDefaultValue();
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
		// back to default value when clearCustomValue() called and happens to be custom value
		if (this->current_value_index == CustomTag::Custom)
		{
			if (enum_custom.first)
				return enum_custom.second;
			else
				this->setDefaultValue();
		}
		return enum_list[this->current_value_index].second;
	}
	constexpr operator ValueType() const noexcept
	{
		// back to default value when clearCustomValue() called and happens to be custom value
		if (this->current_value_index == CustomTag::Custom)
		{
			if (enum_custom.first)
				return *enum_custom.first;
			else
				this->setDefaultValue();
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
	static constexpr ValueType getValueFrom(const CustomEnum& tag_or_var) noexcept
	{
		return tag_or_var;
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

public:
	friend constexpr bool operator==(const CustomEnum& lhs, const CustomEnum& rhs) noexcept
	{
		return lhs.current_value_index == rhs.current_value_index;
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