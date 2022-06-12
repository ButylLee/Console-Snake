#include "Application.h"
#include "Console.h"
#include "Pages.h"
#include "DataSet.h"
#include "GameSaving.h"

#include "wideIO.h"
#include "Resource.h"
#include "EncryptedString.h"
#include "ErrorHandling.h"

#include "WinMacro.h"
#include <Windows.h>
#include <clocale>
#include <cstdlib>
#include <cstdlib>
#include <string>

static bool no_limit = false;

static void ParseCMDAndSet(int count, char* commands[])
{
	using namespace std;
	if (count < 2)
		return;

	string cmd;
	for (int i = 1; i < count; i++)
	{
		cmd = commands[i];
		// Command Options:
		// -nolimit: freely adjust the width and height of Console
		// -oldconsole: enable the compatibility of old console host
		// -awesome: force enable colorful title
		// -size [width]x[height]: set user defined game size
		if (cmd == "-nolimit"_crypt)
		{
			no_limit = true;
		}
		else if (cmd == "-oldconsole"_crypt)
		{
			GameSetting::get().old_console_host = true;
		}
		else if (cmd == "-awesome"_crypt)
		{
			GameData::get().colorful_title = true;
		}
		else if (cmd == "-size"_crypt)
		{
			cmd = commands[++i];
			size_t place = cmd.find('x');
			if (place == string::npos)
				continue;
			try {
				GameSetting::get().width.setCustomValue(abs(stoi(cmd)));
				cmd = cmd.substr(place + 1);
				GameSetting::get().height.setCustomValue(abs(stoi(cmd)));
			}
			catch (...) {
				continue;
			}
		}
	}
}

static void InitConsole()
{
	setlocale(LC_ALL, "");
	try {
		Console::get().moveToScreenCenter();
		Console::get().setConsoleWindow(no_limit ? Console::Sizable : Console::NotSizable, Console::OnlyMin);
		Console::get().setCursorVisible(false);
		Console::get().setTitle(~token::console_title);
	}
	catch (const NativeException& error) {
		print_err(~token::message_init_fail);
		print_err(error.what());
		system("pause");
		exit(EXIT_FAILURE);
	}
}


Application::Application(int argc, char* argv[])
{
	GameSaving::get().convertFromBinaryData();
	ParseCMDAndSet(argc, argv);
	InitConsole();
}

int Application::run()
{
	try {
		while (true)
		{
			auto page = Page::Create();
			page->run();
		}
	}
	catch (const std::bad_alloc&) {
		print_err(~token::message_std_bad_alloc);
	}
	catch (const Exception& error) {
		print_err(error.what());
		GameSaving::get().save();
	}
	catch (const std::exception& error) {
		print_err(error.what());
	}
	catch (...) {
		print_err(~token::message_unknown_error);
		GameSaving::get().save();
	}

	system("pause");
	return EXIT_FAILURE;
}