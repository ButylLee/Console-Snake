#pragma once
#ifndef SNAKE_ERRORHANDLING_HEADER_
#define SNAKE_ERRORHANDLING_HEADER_

#include "WideIO.h"
#include "Resource.h"
#include <string_view>
#include <stdexcept>
#include <cstdlib>
#include <string>
#include "WinHeader.h"

// Base type of all exceptions
class Exception
{
public:
	virtual ~Exception() = default;

	[[nodiscard]] virtual const wchar_t* what() const noexcept = 0;
};

class RuntimeException :public Exception
{
public:
	explicit RuntimeException(std::wstring message) noexcept
		:buffer(std::move(message))
	{}

	[[nodiscard]] const wchar_t* what() const noexcept override
	{
		return buffer.c_str();
	}

private:
	std::wstring buffer;
};

// handle GetLastError result and convert to readable message to throw
class NativeException :public Exception
{
public:
	using CodeType = decltype(GetLastError());
	explicit NativeException(CodeType error_code = GetLastError()) noexcept
		:code(error_code)
	{
		if (!format())
		{
			buffer = L"Format Error Message Failed.";
			code = CodeType{};
		}
	}
	~NativeException() noexcept
	{
		if (code != CodeType{})
			LocalFree((HLOCAL)buffer);
		buffer = nullptr;
	}
	NativeException(const NativeException& other) noexcept :NativeException(other.code) {}
	NativeException& operator=(const NativeException& other) noexcept
	{
		if (this != &other)
			rebuild(other.code);
		return *this;
	}
public:
	[[nodiscard]] const wchar_t* what() const noexcept override
	{
		return buffer;
	}
private:
	bool format() noexcept
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
	void rebuild(CodeType error_code) noexcept
	{
		this->~NativeException();
		(void)new(this) NativeException(error_code);
	}
private:
	const wchar_t* buffer = nullptr;
	CodeType code = {};
};

template<typename T>
inline T* NewWithHandler()
{
	try {
		return new T;
	}
	catch (const std::bad_alloc&) {
		print_err(~Token::message_std_bad_alloc);
		system("pause");
		throw;
	}
	catch (const Exception& error) {
		print_err(error.what());
		system("pause");
		throw;
	}
	catch (const std::exception& error) {
		print_err(error.what());
		system("pause");
		throw;
	}
	catch (...) {
		print_err(~Token::message_unknown_error);
		system("pause");
		throw;
	}
}

#endif // SNAKE_ERRORHANDLING_HEADER_