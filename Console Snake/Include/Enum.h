#pragma once
#ifndef SNAKE_ENUM_HEADER_
#define SNAKE_ENUM_HEADER_

/*
 *                          Enum & CustomEnum
 * Enum and CustomEnum act like built-in enum but can only store pre-defined
 * key-values, whose underlying types are not only integral. It's compile-time
 * in which the pre-defined key-values are defined and the default key-value is
 * specified. CustomEnum can have only one custom value that modified in runtime,
 * which can be null. An Enum object's value can switch circularly among these
 * pre-defined values. It also stores strings with each key-value for reflection.
 *
 * To define an Enum or CustomEnum type like built-in enum:
 *     enum class Size :int{
 *         S = 10,
 *         M = 20,
 *         L = 30
 *     };
 *             ||
 * transform it like below:
 *             ||
 *             vv
 *     struct SizeEnum {
 *         enum Tag {
 *             S, M, L,
 *             DefaultValue = S
 *         };
 *     };
 *     using Size = CustomEnum<SizeEnum, short, std::wstring>; // last two args are optional
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
 *       The macro "ENUM_CUSTOM" is only used for CustomEnum<...>.
 *
 * To use an Enum:
 *     Size size = Size::L;
 *     size.setNextValue();
 *     size.setDefaultValue();
 *     size.convertFrom(20);
 *
 * To get current Name or Value:
 *     auto name = size.Name();
 *     auto value1 = size.Value();
 *     short value2 = size; // implicitly convert
 *
 * To use an CustomEnum:
 *     std::optional<short> pre_custom = Size::GetCustomValue();
 *     Size::ClearCustomValue();
 *     Size::SetCustomValue(50);
 *     size = Size::Custom; // if now has no custom value, fallback to default value
 *
 * To inquire key or value:
 *     auto nameOfS = Size::GetNameFrom(10);
 *     auto valueOfL = Size::GetValueFrom(Size::L);
 */

#include <utility>
#include <vector>
#include <optional>
#include <type_traits>
#include <string>
#include <algorithm>
#include <cassert>

namespace detail {
	template<typename, typename = void, typename = void>
	constexpr bool CheckCorrectEnumInfo = false;
	template<typename Base>
	constexpr bool CheckCorrectEnumInfo<
		Base,
		std::void_t<typename Base::Tag>,
		std::void_t<decltype(Base::DefaultValue)>> = true;
}

// ---------------- Helper Class EnumInfo ---------------
template<typename EnumInfoT, typename ValueT, typename NameT>
struct EnumInfo :EnumInfoT // Derived for enum Tag items
{
	static_assert(detail::CheckCorrectEnumInfo<EnumInfoT>, "Incorrect EnumInfo type.");
	using EnumTag = typename EnumInfoT::Tag;
	using ValueType = std::remove_cv_t<ValueT>;
	using NameType = NameT;
};

// ----------------- Base Class EnumBase ----------------
template<template<typename, typename, typename> typename EnumT,
	typename EnumInfoT, typename ValueT, typename NameT>
class EnumBase :public EnumInfo<EnumInfoT, ValueT, NameT>
{
	using EnumType = EnumT<EnumInfoT, ValueT, NameT>;
	using InfoType = EnumInfo<EnumInfoT, ValueT, NameT>;
public:
	using EnumTag = typename InfoType::EnumTag;
	using ValueType = typename InfoType::ValueType;
	using NameType = typename InfoType::NameType;

public:
	EnumBase() = default;
	// EnumTag literals could implicitly cast to CustomEnum
	EnumBase(EnumTag tag) noexcept
	{
		current_value_index = static_cast<size_t>(tag);
	}

public:
	const EnumType& setNextValue() noexcept /* virtual */
	{
		return static_cast<EnumType*>(this)->setNextValue();
	}
	const EnumType& setDefaultValue() noexcept
	{
		current_value_index = EnumTag::DefaultValue;
		return static_cast<const EnumType&>(*this);
	}
	bool convertFrom(ValueType val) noexcept /* virtual */
	{
		return static_cast<EnumType*>(this)->convertFrom(val);
	}
	NameType Name() const noexcept /* virtual */
	{
		return static_cast<const EnumType*>(this)->Name();
	}
	operator ValueType() const noexcept /* virtual */
	{
		return static_cast<const EnumType*>(this)->operator ValueType();
	}
	ValueType Value() const noexcept
	{
		return static_cast<ValueType>(*this); // invoke operator value_type
	}

public:
	static NameType GetNameFrom(ValueType val) noexcept /* virtual */
	{
		return EnumType::GetNameFrom(val);
	}
	static ValueType GetValueFrom(NameType name) noexcept /* virtual */
	{
		return EnumType::GetValueFrom(name);
	}

public:
	friend bool operator==(const EnumBase& lhs, const EnumBase& rhs) noexcept
	{
		return lhs.current_value_index == rhs.current_value_index;
	}
	friend bool operator!=(const EnumBase& lhs, const EnumBase& rhs) noexcept
	{
		return !(lhs == rhs);
	}
	friend bool operator<(const EnumBase& lhs, const EnumBase& rhs) noexcept
	{
		return lhs.current_value_index < rhs.current_value_index;
	}
	friend bool operator>(const EnumBase& lhs, const EnumBase& rhs) noexcept
	{
		return rhs < lhs;
	}
	friend bool operator<=(const EnumBase& lhs, const EnumBase& rhs) noexcept
	{
		return !(lhs > rhs);
	}
	friend bool operator>=(const EnumBase& lhs, const EnumBase& rhs) noexcept
	{
		return !(lhs < rhs);
	}

protected:
	std::make_signed_t<size_t> current_value_index = EnumTag::DefaultValue;
};

// ------------------- Main Class Enum ------------------
template<typename EnumInfoT, typename ValueT = int, typename NameT = std::wstring>
class Enum :
	public EnumBase<Enum, EnumInfoT, ValueT, NameT>
{
	using Base = EnumBase<Enum, EnumInfoT, ValueT, NameT>;
public:
	using typename Base::EnumTag;
	using typename Base::ValueType;
	using typename Base::NameType;
private:
	using pair_type = std::pair<ValueType, std::add_const_t<NameType>>;
	using list_type = const std::vector<pair_type>;

public:
	Enum() = default;
	Enum(EnumTag tag) noexcept
		:Base(tag)
	{}

public:
	const Enum& setNextValue() noexcept
	{
		if (this->current_value_index == enum_list.size() - 1)
			this->current_value_index = 0;
		else
			this->current_value_index++;
		return *this;
	}
	bool convertFrom(ValueType val) noexcept
	{
		auto iter = std::ranges::find_if(enum_list,
										 [&](const pair_type& item)
										 {
											 return val == item.first;
										 });
		if (iter != enum_list.cend())
		{
			this->current_value_index = iter - enum_list.cbegin();
			return true;
		}
		return false;
	}
	NameType Name() const noexcept
	{
		return enum_list[this->current_value_index].second;
	}
	operator ValueType() const noexcept
	{
		return enum_list[this->current_value_index].first;
	}

public:
	static NameType GetNameFrom(ValueType val) noexcept
	{
		auto iter = std::ranges::find_if(enum_list,
										 [&](const pair_type& item)
										 {
											 return val == item.first;
										 });
		if (iter != enum_list.cend())
			return (*iter).second;
		return {};
	}
	static ValueType GetValueFrom(NameType name) noexcept
	{
		auto iter = std::ranges::find_if(enum_list,
										 [&](const pair_type& item)
										 {
											 return name == item.second;
										 });
		if (iter != enum_list.cend())
			return (*iter).first;
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
	public CustomInfo,
	public EnumBase<CustomEnum, EnumInfoT, ValueT, NameT>
{
	using Base = EnumBase<CustomEnum, EnumInfoT, ValueT, NameT>;
public:
	using typename Base::EnumTag;
	using typename Base::ValueType;
	using typename Base::NameType;
private:
	using pair_type = std::pair<ValueType, std::add_const_t<NameType>>;
	using list_type = const std::vector<pair_type>;
	using custom_pair_type = std::pair<std::optional<ValueType>, std::add_const_t<NameType>>;

public:
	CustomEnum() = default;
	CustomEnum(EnumTag tag) noexcept
		:Base(tag)
	{}
	CustomEnum(CustomTag tag) noexcept
		:Base(static_cast<EnumTag>(tag))
	{}

public:
	const CustomEnum& setNextValue() noexcept
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
	bool convertFrom(ValueType val) noexcept
	{
		auto iter = std::ranges::find_if(enum_list,
										 [&](const pair_type& item)
										 {
											 return val == item.first;
										 });
		if (iter != enum_list.cend())
		{
			this->current_value_index = iter - enum_list.cbegin();
			return true;
		}
		SetCustomValue(val);
		this->current_value_index = CustomTag::Custom;
		return true;
	}
	NameType Name() const noexcept
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
	operator ValueType() const noexcept
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

public:
	static NameType GetNameFrom(ValueType val) noexcept
	{
		if (val == enum_custom.first)
			return enum_custom.second;
		auto iter = std::ranges::find_if(enum_list,
										 [&](const pair_type& item)
										 {
											 return val == item.first;
										 });
		if (iter != enum_list.cend())
			return (*iter).second;
		return {};
	}
	static ValueType GetValueFrom(NameType name) noexcept
	{
		if (name == enum_custom.second)
			return enum_custom.first;
		auto iter = std::ranges::find_if(enum_list,
										 [&](const pair_type& item)
										 {
											 return name == item.second;
										 });
		if (iter != enum_list.cend())
			return (*iter).first;
		return {};
	}

	static void SetCustomValue(ValueType custom)
	{
		enum_custom.first = std::move(custom);
	}
	static std::optional<ValueType> GetCustomValue() noexcept
	{
		return enum_custom.first;
	}
	static void ClearCustomValue() noexcept
	{
		enum_custom.first = std::nullopt;
	}

private:
	// back to default value when clearCustomValue() called and happens to be custom value
	void setDefaultValue_force() const noexcept
	{
		const_cast<CustomEnum*>(this)->setDefaultValue();
	}

private:
	static list_type enum_list;
	static custom_pair_type enum_custom;
};

// ------------- Main Class MultiCustomEnum -------------
template<typename EnumInfoT, typename ValueT = int, typename NameT = std::wstring>
class MultiCustomEnum :
	public EnumBase<MultiCustomEnum, EnumInfoT, ValueT, NameT>
{
	using Base = EnumBase<MultiCustomEnum, EnumInfoT, ValueT, NameT>;
public:
	using typename Base::EnumTag;
	using typename Base::ValueType;
	using typename Base::NameType;
private:
	using pair_type = std::pair<ValueType, std::add_const_t<NameType>>;
	using list_type = const std::vector<pair_type>;
	using custom_list_type = std::vector<pair_type>;
	using notify_list_type = std::vector<MultiCustomEnum*>;

public:
	MultiCustomEnum()
	{
		notify_list.push_back(this);
	}
	MultiCustomEnum(EnumTag tag)
		:Base(tag)
	{
		notify_list.push_back(this);
	}
	~MultiCustomEnum() noexcept
	{
		auto iter = std::ranges::find(notify_list, this);
		assert(iter != notify_list.crend());
		notify_list.erase(iter);
	}

public:
	const MultiCustomEnum& setNextValue() noexcept
	{
		if (this->current_value_index == enum_list.size() + custom_list.size() - 1)
			this->current_value_index = 0;
		else
			this->current_value_index++;
		return *this;
	}
	bool convertFrom(ValueType val) noexcept
	{
		auto pred = [&](const pair_type& item) {
			return val == item.first;
		};
		auto iter = std::ranges::find_if(enum_list, pred);
		if (iter != enum_list.cend())
		{
			this->current_value_index = iter - enum_list.cbegin();
			return true;
		}
		iter = std::ranges::find_if(custom_list, pred);
		if (iter != custom_list.cend())
		{
			this->current_value_index = iter - custom_list.cbegin() + enum_list.size();
			return true;
		}
		return false;
	}
	NameType Name() const noexcept
	{
		pair_type& item = FetchEnumItem(this->current_value_index);
		return item.second;
	}
	operator ValueType() const noexcept
	{
		pair_type& item = FetchEnumItem(this->current_value_index);
		return item.first;
	}

public:
	static NameType GetNameFrom(ValueType val) noexcept
	{
		auto pred = [&](const pair_type& item) {
			return val == item.first;
		};
		auto iter = std::ranges::find_if(enum_list, pred);
		if (iter != enum_list.cend())
			return (*iter).second;
		iter = std::ranges::find_if(custom_list, pred);
		if (iter != custom_list.cend())
			return (*iter).second;
		return {};
	}
	static ValueType GetValueFrom(NameType name) noexcept
	{
		auto pred = [&](const pair_type& item) {
			return name == item.second;
		};
		auto iter = std::ranges::find_if(enum_list, pred);
		if (iter != enum_list.cend())
			return (*iter).first;
		iter = std::ranges::find_if(custom_list, pred);
		if (iter != custom_list.cend())
			return (*iter).first;
		return {};
	}

	static void AddCustomItem(ValueType val, NameType name)
	{
		custom_list.emplace_back(val, name);
	}
	static void RemoveCustomItem(NameType name) noexcept
	{
		auto iter = std::ranges::find_if(custom_list,
										 [&](const pair_type& item)
										 {
											 return item.second == name;
										 });
		if (iter == custom_list.cend())
			return;
		size_t index = iter - custom_list.cbegin() + enum_list.size();
		custom_list.erase(iter);
		NotifyAllObject(index);
	}

private:
	static pair_type& FetchEnumItem(size_t index) noexcept
	{
		assert(index < enum_list.size() + custom_list.size());
		if (index < enum_list.size())
			return enum_list[index];
		else
			return custom_list[index - enum_list.size()];
	}
	static void NotifyAllObject(size_t removed_index) noexcept
	{
		for (auto& obj : notify_list)
		{
			if (obj->current_value_index == removed_index)
				obj->setDefaultValue();
			else if (obj->current_value_index > removed_index)
				obj->current_value_index--;
		}
	}

private:
	static list_type enum_list;
	static custom_list_type custom_list;
	static notify_list_type notify_list;
};

#define ENUM_DEFINE(name) \
inline name::list_type name::enum_list

#define ENUM_CUSTOM(name) \
inline name::custom_pair_type name::enum_custom

#endif // SNAKE_ENUM_HEADER_