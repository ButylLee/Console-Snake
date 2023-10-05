#pragma once
#ifndef SNAKE_CONSOLE_HEADER_
#define SNAKE_CONSOLE_HEADER_

#include "Modules.h"
#include "WinHeader.h"
#include "Property.h"
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

private:
	void setWindowAttribute(LONG_PTR args);

private:
	LONG_PTR getWindowAttribute() const;
	HANDLE fetchOutputHandle() const;
	HWND fetchConsoleHandle() const;

public:
	UsingProperty(ConsoleBase);
	Property<HWND, Get> console_handle;
	Property<HANDLE, Get> output_handle;

private:
	std::wstring title;
};

using Console = ModuleRegister<ConsoleBase>;

#endif // SNAKE_CONSOLE_HEADER_