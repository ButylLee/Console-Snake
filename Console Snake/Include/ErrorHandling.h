#pragma once
#ifndef SNAKE_ERRORHANDLING_HEADER_
#define SNAKE_ERRORHANDLING_HEADER_

#include "wideIO.h"
#include "Resource.h"
#include <new>
#include <string_view>
#include <stdexcept>
#include <cstdlib>
#include "WinMacro.h"
#include <Windows.h>

// handle GetLastError result and convert to readable message to throw
class NativeError
{
	using code_type = decltype(GetLastError());
public:
	explicit NativeError(DWORD error_code) :code(error_code)
	{
		if (!format())
			throw NativeError(GetLastError());
	}
	explicit NativeError(const wchar_t* message) noexcept
		:buffer(message)
	{}
	NativeError(std::wstring_view message) noexcept
		:buffer(message.data())
	{}
	~NativeError() noexcept
	{
		LocalFree(const_cast<WCHAR*>(buffer));
		buffer = nullptr;
	}
	NativeError(const NativeError& other) :NativeError(other.code) {}
	NativeError& operator=(const NativeError& other)
	{
		if (this == &other)
		{
			return *this;
		}
		rebuild(other.code);
		return *this;
	}
public:
	[[nodiscard]] const wchar_t* what() const noexcept
	{
		return buffer;
	}
private:
	bool format() const noexcept
	{
		return static_cast<bool>(
			FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
						   FORMAT_MESSAGE_FROM_SYSTEM |
						   FORMAT_MESSAGE_IGNORE_INSERTS,
						   NULL,
						   code,
						   LANG_USER_DEFAULT,
						   (LPWSTR)&buffer,
						   0,
						   NULL)
			);
	}
	void rebuild(code_type error_code)
	{
		this->~NativeError();
		(void)new(this) NativeError(error_code);
	}
private:
	code_type code = {};
	const WCHAR* buffer = nullptr; // mutable in FormatMessageW call
};

template<typename T>
inline T* NewWithHandler() noexcept
{
	try {
		return new T;
	}
	catch (const std::bad_alloc&) {
		print_err(~token::message_std_bad_alloc);
	}
	catch (const NativeError& error) {
		print_err(error.what());
	}
	catch (const std::exception& error) {
		print_err(error.what());
	}
	catch (...) {
		print_err(~token::message_unknown_error);
	}

	system("pause");
	exit(EXIT_FAILURE);
}

#endif // SNAKE_ERRORHANDLING_HEADER_