#include "Console.h"
#include "ErrorHandling.h"
#include "wideIO.h"
#include "Resource.h"
#include <utility>
#include <cstdlib>

ConsoleBase::ConsoleBase() try
{
	hOutput = fetchOutputHandle();
	hConsole = fetchConsoleHandle();
	// set initial window style
	setWindowAttribute(WS_VISIBLE | WS_CAPTION | WS_SYSMENU |
					   WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX);
}
catch (const Exception&) {
	print_err(~token::message_init_console_fail);
	throw;
}

void ConsoleBase::setTitle(std::wstring new_title)
{
	if (!SetConsoleTitleW(new_title.c_str()))
		throw NativeException{};
	title = std::move(new_title);
}

void ConsoleBase::setConsoleWindow(CanSize cansize, CanMinMax canminmax)
{
	static constexpr LONG_PTR mask = Sizable | MinMax;

	LONG_PTR win_att = getWindowAttribute();
	win_att &= ~mask;
	win_att |= cansize;
	win_att |= canminmax;
	setWindowAttribute(win_att);
}

void ConsoleBase::setConsoleWindow(HasFrame hasframe)
{
	static constexpr LONG_PTR mask = UseFrame;

	LONG_PTR win_att = getWindowAttribute();
	win_att &= ~mask;
	win_att |= hasframe;
	setWindowAttribute(win_att);
}

void ConsoleBase::setCursorVisible(bool isVisible)
{
	CONSOLE_CURSOR_INFO cci;
	if (!GetConsoleCursorInfo(hOutput, &cci))
		throw NativeException{};
	cci.bVisible = static_cast<BOOL>(isVisible);
	if (!SetConsoleCursorInfo(hOutput, &cci))
		throw NativeException{};
}

void ConsoleBase::moveToScreenCenter() noexcept
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

HWND ConsoleBase::getConsoleHandle() const noexcept
{
	return hConsole;
}

HANDLE ConsoleBase::getOutputHandle() const noexcept
{
	return hOutput;
}

void ConsoleBase::setWindowAttribute(LONG_PTR args)
{
	if (!SetWindowLongPtrW(hConsole, GWL_STYLE, args))
		throw NativeException{};
}

LONG_PTR ConsoleBase::getWindowAttribute() const
{
	LONG_PTR win_att = GetWindowLongPtrW(hConsole, GWL_STYLE);
	if (win_att == 0)
		throw NativeException{};
	return win_att;
}

HANDLE ConsoleBase::fetchOutputHandle() const
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (handle == INVALID_HANDLE_VALUE)
		throw NativeException{};
	return handle;
}

HWND ConsoleBase::fetchConsoleHandle() const
{
	HWND hwnd = GetConsoleWindow();
	if (hwnd == NULL)
		throw RuntimeException(~token::GetConsoleWindow_failed_message);
	return hwnd;
}