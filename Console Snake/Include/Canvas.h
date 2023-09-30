#pragma once
#ifndef SNAKE_CANVAS_HEADER_
#define SNAKE_CANVAS_HEADER_

#include "Resource.h"
#include "WinHeader.h"
#include "Interface.h"
#include <string_view>
#include <stack>

struct Cursor
{
public:
	Cursor() = default;
	constexpr Cursor(short x, short y) noexcept :x(x), y(y) {}
	constexpr operator COORD() const noexcept
	{
		return COORD{
			// the height(y) of character is about twice than width(x)
			static_cast<SHORT>(x * 2),
			static_cast<SHORT>(y)
		};
	}
	constexpr Cursor& operator+=(const Cursor& offset) noexcept
	{
		this->x += offset.x;
		this->y += offset.y;
		return *this;
	}
	constexpr Cursor& operator-=(const Cursor& offset) noexcept
	{
		this->x -= offset.x;
		this->y -= offset.y;
		return *this;
	}
	friend constexpr Cursor operator+(Cursor lhs, const Cursor& rhs) noexcept
	{
		return lhs += rhs;
	}
	friend constexpr Cursor operator-(Cursor lhs, const Cursor& rhs) noexcept
	{
		return lhs -= rhs;
	}

public:
	short x = 0;
	short y = 0;
};

struct ClientSize
{
	short width;   // X
	short height;  // Y
};

class Canvas :NotCopyable
{
public:
	Canvas();

public:
	void setColor(Color new_color);
	void setCursor(short newX, short newY);
	void pushCursorOffset(short X, short Y) noexcept;
	void popCursorOffset() noexcept;
	void setCursorCentered(std::wstring_view str, short newY);
	void setClientSize(short width, short height) noexcept;
	void setClientSize(ClientSize new_size) noexcept;
	void nextLine();
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
	Cursor offset;
	std::stack<Cursor> offset_stack;
	ClientSize size = { 45, 35 };
};

#endif // SNAKE_CANVAS_HEADER_