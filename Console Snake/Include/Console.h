#pragma once
#ifndef SNAKE_CONSOLE_HEADER_
#define SNAKE_CONSOLE_HEADER_

#include "Singleton.h"
#include "WinMacro.h"
#include <Windows.h>
#include <string>

class ConsoleBase
{
public:
	enum CanSize :LONG_PTR
	{
		NotSizable,
		Sizable = WS_SIZEBOX
	};
	enum CanMinMax :LONG_PTR
	{
		NoMinMax,
		OnlyMin = WS_MINIMIZEBOX,
		OnlyMax = WS_MAXIMIZEBOX,
		MinMax = WS_MINIMIZEBOX | WS_MAXIMIZEBOX
	};
	enum HasFrame :LONG_PTR
	{
		NoFrame,
		UseFrame = WS_DLGFRAME
	};

protected:
	ConsoleBase();

public:
	void setTitle(std::wstring new_title);
	void setConsoleWindow(CanSize cansize, CanMinMax canminmax);
	void setConsoleWindow(HasFrame hasframe);
	void setCursorVisible(bool isVisible);
	void moveToScreenCenter() noexcept;

public:
	HWND getConsoleHandle() const noexcept;
	HANDLE getOutputHandle() const noexcept;

private:
	void setWindowAttribute(LONG_PTR args);

private:
	LONG_PTR getWindowAttribute() const;
	HANDLE fetchOutputHandle() const;
	HWND fetchConsoleHandle() const;

private:
	HWND hConsole;
	HANDLE hOutput;
	std::wstring title;
};

using Console = Singleton<ConsoleBase>;

#endif // SNAKE_CONSOLE_HEADER_