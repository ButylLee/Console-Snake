#pragma once
#ifndef SNAKE_CANVAS_HEADER_
#define SNAKE_CANVAS_HEADER_

#include "Console.h"
#include "ErrorHandling.h"
#include "EncryptedString.h"
#include "wideIO.h"
#include "WinMacro.h"
#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <string_view>

enum struct Color :WORD
{
	Gray   = 0x08,
	Blue   = 0x01, LightBlue   = 0x09,
	Green  = 0x02, LightGreen  = 0x0A,
	Aqua   = 0x03, LightAqua   = 0x0B,
	Red    = 0x04, LightRed    = 0x0C,
	Purple = 0x05, LightPurple = 0x0D,
	Yellow = 0x06, LightYellow = 0x0E,
	White  = 0x07, LightWhite  = 0x0F,
};

class Cursor
{
public:
	Cursor() = default;
	constexpr Cursor(short x, short y) noexcept :x(x), y(y) {}
public:
	constexpr operator COORD() const noexcept
	{
		return COORD{
			// the height(y) of character is about twice than width(x)
			static_cast<SHORT>(x * 2),
			static_cast<SHORT>(y)
		};
	}
public:
	constexpr short getX() const noexcept { return x; }
	constexpr short getY() const noexcept { return y; }
private:
	short x = 0;
	short y = 0;
};

struct ClientSize
{
	short width;   // X
	short height;  // Y
};

class Canvas
{
public:
	Canvas()
	{
		setColor();
		setCursor();
	}

public:
	void setColor(Color new_color)
	{
		if (color == new_color)
			return;
		color = new_color;
		setColor();
	}
	void setCursor(short newX, short newY)
	{
		cursor = { newX,newY };
		setCursor();
	}
	void setCenteredCursor(const std::wstring_view& str, short newY)
	{
		cursor = { calCenteredCoord(str),newY };
		setCursor();
	}
	void setClientSize(short width, short height) noexcept
	{
		size.width = width;
		size.height = height;
		setClientSize();
	}
	void setClientSize(ClientSize new_size)
	{
		size = new_size;
		setClientSize();
	}
	void clear() noexcept
	{
		setClientSize(); // use side effect
	}
public:
	ClientSize getClientSize() const noexcept
	{
		return size;
	}
	Color getColor() const noexcept
	{
		return color;
	}

private:
	void setColor()
	{
		if (!SetConsoleTextAttribute(Console::get().getOutputHandle(), static_cast<WORD>(color)))
			throw NativeError(GetLastError());
	}
	void setCursor()
	{
		if (!SetConsoleCursorPosition(Console::get().getOutputHandle(), cursor))
			throw NativeError(GetLastError());
	}
	void setClientSize() noexcept
	{
		char con[33];
		sprintf_s(con, "mode con: cols=%d lines=%d"_crypt.c_str(), size.width * 2, size.height);
		system(con); // side effect: clear screen
	}
	// calculate the correct left x of centered string
	short calCenteredCoord(const std::wstring_view& str) const noexcept
	{
		short coord = (size.width - static_cast<short>(StrFullWidthLen(str))) / 2;
		return coord > 0 ? coord : 0;
	}

private:
	Color color = Color::White;
	Cursor cursor;
	ClientSize size = { 60, 30 };
};

#endif // SNAKE_CANVAS_HEADER_