#pragma once
#ifndef SNAKE_CANVAS_HEADER_
#define SNAKE_CANVAS_HEADER_

#include "Resource.h"
#include "WinMacro.h"
#include <Windows.h>
#include <string_view>

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
	Canvas();
	Canvas(const Canvas&) = delete;
	Canvas& operator=(const Canvas&) = delete;

public:
	void setColor(Color new_color);
	void setCursor(short newX, short newY);
	void setCenteredCursor(std::wstring_view str, short newY);
	void setClientSize(short width, short height) noexcept;
	void setClientSize(ClientSize new_size);
	void clear() noexcept;

public:
	ClientSize getClientSize() const noexcept;
	Color getColor() const noexcept;

private:
	void applyColor();
	void applyCursor();
	void applyClientSize() noexcept;
	// calculate the correct left x of centered string
	short calCenteredCoord(const std::wstring_view& str) const noexcept;

private:
	Color color = Color::White;
	Cursor cursor;
	ClientSize size = { 60, 30 };
};

#endif // SNAKE_CANVAS_HEADER_