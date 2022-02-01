#include "Canvas.h"
#include "Console.h"
#include "ErrorHandling.h"
#include "EncryptedString.h"
#include <cstdio>
#include <cstdlib>

Canvas::Canvas()
{
	applyColor();
	applyCursor();
}

void Canvas::setColor(Color new_color)
{
	if (color == new_color)
		return;
	color = new_color;
	applyColor();
}

void Canvas::setCursor(short newX, short newY)
{
	cursor = { newX, newY };
	applyCursor();
}

void Canvas::setCenteredCursor(std::wstring_view str, short newY)
{
	cursor = { calCenteredCoord(str), newY };
	applyCursor();
}

void Canvas::setClientSize(short width, short height) noexcept
{
	size.width = width;
	size.height = height;
	applyClientSize();
}

void Canvas::setClientSize(ClientSize new_size)
{
	size = new_size;
	applyClientSize();
}

void Canvas::clear() noexcept
{
	applyClientSize(); // use side effect
}

ClientSize Canvas::getClientSize() const noexcept
{
	return size;
}

Color Canvas::getColor() const noexcept
{
	return color;
}

void Canvas::applyColor()
{
	if (!SetConsoleTextAttribute(Console::get().getOutputHandle(), static_cast<WORD>(color)))
		throw NativeError(GetLastError());
}

void Canvas::applyCursor()
{
	if (!SetConsoleCursorPosition(Console::get().getOutputHandle(), cursor))
		throw NativeError(GetLastError());
}

void Canvas::applyClientSize() noexcept
{
	char con[33];
	sprintf_s(con, "mode con: cols=%d lines=%d"_crypt.c_str(), size.width * 2, size.height);
	system(con); // side effect: clear screen
}

short Canvas::calCenteredCoord(const std::wstring_view& str) const noexcept
{
	short coord = (size.width - static_cast<short>(StrFullWidthLen(str))) / 2;
	return coord > 0 ? coord : 0;
}