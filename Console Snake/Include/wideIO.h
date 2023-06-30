#pragma once
#ifndef SNAKE_WIDEIO_HEADER_
#define SNAKE_WIDEIO_HEADER_

#include <utility>
#include <cstdint>
#include <cwchar>
#include <cwctype>
#include <climits>
#include <string>
#include <string_view>
#include <format>
#include <algorithm>

using wint = uint32_t;
static_assert(sizeof(wint) == 2 * sizeof(wint_t));

/***************************************
 Function: common print
****************************************/
inline auto print(wchar_t ch) noexcept
{
	return putwchar(ch);
}

template<typename... TArgs>
inline auto print(std::wstring_view format, TArgs&&... args) noexcept
{
	return wprintf_s(format.data(), std::forward<TArgs>(args)...);
}

/***************************************
 Function: print error message
****************************************/
template<typename... TArgs>
inline auto print_err(std::wstring_view format, TArgs&&... args) noexcept
{
	return fwprintf_s(stderr, format.data(), std::forward<TArgs>(args)...);
}

// print_err ASCII verion for std::exception
template<typename... TArgs>
inline auto print_err(std::string_view format, TArgs&&... args) noexcept
{
	return fprintf_s(stderr, format.data(), std::forward<TArgs>(args)...);
}

/***************************************
 Function: get wide char without echo
****************************************/
[[nodiscard]]
inline wint getwch() noexcept
{
	// figure out special key that return 2 values
	// the key definition enums are in KeyMap.h
	wint key = _getwch();
	if (key == 224 || key == 0)
	{
		key |= _getwch() << sizeof(wint_t) * CHAR_BIT;
	}
	return key;
}

/***************************************
 Function: unget wide char
****************************************/
void ungetwch(wint ch) noexcept;

/***************************************
 Function: wrapper of std::format
****************************************/
template<typename... TArgs>
inline std::wstring format(std::wstring_view fmt, TArgs&&... args)
{
	return std::vformat(fmt, std::make_wformat_args(std::forward<TArgs>(args)...));
}

/***************************************
 Function: calculate the full-width length
****************************************/
constexpr size_t StrFullWidthLen(std::wstring_view str) noexcept
{
	size_t half_width_count = std::ranges::count_if(
		str, [](wchar_t ch)
		{
			// select ASCII printable characters, which are half-width
			return !(ch >> sizeof(wchar_t) * CHAR_BIT / 2) && iswprint(ch);
		}
	);
	half_width_count /= 2;
	return str.length() - half_width_count;
}

/***************************************
 Function: count the full-width characters
****************************************/
constexpr size_t StrFullWidthCount(std::wstring_view str) noexcept
{
	return std::ranges::count_if(
		str, [](wchar_t ch)
		{
			return !!(ch >> sizeof(wchar_t) * CHAR_BIT / 2) && iswprint(ch);
		}
	);
}

#endif // SNAKE_WIDEIO_HEADER_