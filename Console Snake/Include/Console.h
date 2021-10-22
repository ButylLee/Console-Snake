#pragma once
#ifndef SNAKE_CONSOLE_HEADER_
#define SNAKE_CONSOLE_HEADER_

#include "ErrorHandling.h"
#include "wideIO.h"
#include "Resource.h"
#include "Singleton.h"
#include "WinMacro.h"
#include <Windows.h>
#include <string>
#include <utility>
#include <cstdlib>

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
	ConsoleBase() try
	{
		hOutput = fetchOutputHandle();
		hConsole = fetchConsoleHandle();
		// set initial window style
		setWindowAttribute(WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_CHILD |
						   WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX);
	}
	catch (const NativeError&) {
		print_err(~token::message_init_console_fail);
		throw;
	}

public:
	void setTitle(std::wstring new_title)
	{
		if (!SetConsoleTitleW(new_title.c_str()))
			throw NativeError(GetLastError());
		title = std::move(new_title);
	}
	void setConsoleWindow(CanSize cansize, CanMinMax canminmax)
	{
		static constexpr LONG_PTR mask = Sizable | MinMax;

		LONG_PTR win_att = getWindowAttribute();
		win_att &= ~mask;
		win_att |= cansize;
		win_att |= canminmax;
		setWindowAttribute(win_att);
	}
	void setConsoleWindow(HasFrame hasframe)
	{
		static constexpr LONG_PTR mask = UseFrame;

		LONG_PTR win_att = getWindowAttribute();
		win_att &= ~mask;
		win_att |= hasframe;
		setWindowAttribute(win_att);
	}
	void setCursorVisible(bool isVisible)
	{
		CONSOLE_CURSOR_INFO cci;
		if (!GetConsoleCursorInfo(hOutput, &cci))
			throw NativeError(GetLastError());
		cci.bVisible = static_cast<BOOL>(isVisible);
		if (!SetConsoleCursorInfo(hOutput, &cci))
			throw NativeError(GetLastError());
	}
	void moveToScreenCenter() noexcept
	{
		RECT window;
		if (!GetWindowRect(hConsole, &window))
			return;
		auto width = window.right - window.left;
		auto height = window.bottom - window.top;

		MONITORINFO info;
		info.cbSize = sizeof(MONITORINFO);
		if (!GetMonitorInfoW(MonitorFromWindow(hConsole, MONITOR_DEFAULTTONULL), &info))
			return;
		RECT screen = info.rcWork;
		auto left = screen.left + (screen.right - screen.left - width) / 2;
		auto top = screen.top + (screen.bottom - screen.top - height) / 2;

		// side effect: restore ConsoleWindow attributes
		MoveWindow(hConsole, left, top, width, height, TRUE);
	}

public:
	HWND getConsoleHandle() const noexcept
	{
		return hConsole;
	}
	HANDLE getOutputHandle() const noexcept
	{
		return hOutput;
	}

private:
	void setWindowAttribute(LONG_PTR args)
	{
		if (!SetWindowLongPtrW(hConsole, GWL_STYLE, args))
			throw NativeError(GetLastError());
	}

private:
	LONG_PTR getWindowAttribute() const
	{
		LONG_PTR win_att = GetWindowLongPtrW(hConsole, GWL_STYLE);
		if (win_att == 0)
			throw NativeError(GetLastError());
		return win_att;
	}
	HANDLE fetchOutputHandle() const
	{
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (handle == INVALID_HANDLE_VALUE)
			throw NativeError(GetLastError());
		return handle;
	}
	HWND fetchConsoleHandle() const
	{
		HWND hwnd = GetConsoleWindow();
		if (hwnd == NULL)
			throw NativeError(~token::GetConsoleWindow_failed_message);
		return hwnd;
	}

private:
	HWND hConsole;
	HANDLE hOutput;
	std::wstring title;
};

using Console = Singleton<ConsoleBase>;

#endif // SNAKE_CONSOLE_HEADER_