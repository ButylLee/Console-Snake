#pragma once
#ifndef SNAKE_WIDEIO_HEADER_
#define SNAKE_WIDEIO_HEADER_

#include <utility>
#include <cwchar>
#include <cwctype>
#include <climits>
#include <string>
#include <string_view>

using wint = unsigned int;
static_assert(sizeof(wint) == 2 * sizeof(wint_t));

/***************************************
 Function: common print
****************************************/
inline auto print(wchar_t ch) noexcept
{
	return putwchar(ch);
}

template<typename... TArgs>
inline auto print(const wchar_t* format, TArgs&&... args) noexcept
{
	return wprintf_s(format, std::forward<TArgs>(args)...);
}

template<typename... TArgs>
inline auto print(std::wstring format, TArgs&&... args) noexcept
{
	return wprintf_s(format.c_str(), std::forward<TArgs>(args)...);
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
inline auto print_err(const wchar_t* format, TArgs&&... args) noexcept
{
	return fwprintf_s(stderr, format, std::forward<TArgs>(args)...);
}

template<typename... TArgs>
inline auto print_err(std::wstring format, TArgs&&... args) noexcept
{
	return fwprintf_s(stderr, format.c_str(), std::forward<TArgs>(args)...);
}

template<typename... TArgs>
inline auto print_err(std::wstring_view format, TArgs&&... args) noexcept
{
	return fwprintf_s(stderr, format.data(), std::forward<TArgs>(args)...);
}

// print_err ASCII verion for std::exception
template<typename... TArgs>
inline auto print_err(const char* format, TArgs&&... args) noexcept
{
	return fprintf_s(stderr, format, std::forward<TArgs>(args)...);
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
 Function: calculate the full-width length
****************************************/
constexpr size_t StrFullWidthLen(std::wstring_view str) noexcept
{
	int half_width_count = 0;
	for (const wchar_t& ch : str)
	{
		// select ASCII printable characters, which are half-width
		if (!(ch >> sizeof(wchar_t) * CHAR_BIT / 2) && iswprint(ch))
			half_width_count++;
	}
	half_width_count /= 2;
	return str.length() - half_width_count;
}

/***************************************
 Function: count the full-width characters
****************************************/
constexpr size_t StrFullWidthCount(std::wstring_view str) noexcept
{
	int full_width_count = 0;
	for (const wchar_t& ch : str)
	{
		if (!!(ch >> sizeof(wchar_t) * CHAR_BIT / 2) && iswprint(ch))
			full_width_count++;
	}
	return full_width_count;
}

#endif // SNAKE_WIDEIO_HEADER_