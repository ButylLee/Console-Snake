#include "Application.h"
#include "Canvas.h"
#include "Console.h"
#include "Pages.h"
#include "GameSaving.h"
#include "WideIO.h"
#include "ErrorHandling.h"
#include "LocalizedStrings.h"
#include "EncryptedString.h"
#include "Pythonic.h"
#include "GlobalData.h"

#include "WinHeader.h"
#include <clocale>
#include <cstdlib>
#include <string>

namespace {
	bool no_limit = false;

	void ParseCMDAndSet(int count, char* commands[])
	{
		using namespace std;
		if (count < 2)
			return;

		string cmd;
		for (auto i : range(1, count))
		{
			cmd = commands[i];
			// Command Options:
			// -nolimit: freely adjust the width and height of Console
			// -oldconsole: enable the compatibility of old console host
			// -awesome: force enable colorful title
			if (cmd == "-nolimit"_crypt)
			{
				no_limit = true;
			}
			else if (cmd == "-oldconsole"_crypt)
			{
				GameSetting::get().old_console_host = true;
				GameSetting::get().show_frame = true;
			}
			else if (cmd == "-awesome"_crypt)
			{
				GameData::get().colorful_title = true;
			}
		}
	}

	void InitConsole()
	{
		setlocale(LC_ALL, "");
		try {
			Canvas fake_canvas; // set default size before move
			fake_canvas.setClientSize(fake_canvas.getClientSize());
			Console::get().moveToScreenCenter();
			Console::get().setConsoleWindow(no_limit ? Console::Sizable : Console::NotSizable, Console::OnlyMin);
			Console::get().setCursorVisible(false);
			Console::get().setTitle(~Token::console_title);
		}
		catch (const NativeException& error) {
			print_err(~Token::message_init_fail);
			print_err(error.what());
			system("pause");
			throw;
		}
	}
}

void EnsureOnlyOneInstance() noexcept
{
#if !defined(_DEBUG) && defined(NDEBUG)
	HANDLE handle = CreateMutex(NULL, FALSE, L"Local\\ConsoleSnakeButylLee23"_crypt.c_str());
	if (handle == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
		exit(EXIT_FAILURE);
#endif
}

Application::Application(int argc, char* argv[])
{
	GameSaving::get().convertFromSaveData();
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
			if (GameData::get().exit_game)
				return EXIT_SUCCESS;
		}
	}
	catch (const std::bad_alloc&) {
		print_err(~Token::message_std_bad_alloc);
	}
	catch (const Exception& error) {
		print_err(error.what());
		GameSaving::get().save();
	}
	catch (const std::exception& error) {
		print_err(error.what());
	}
	catch (...) {
		print_err(~Token::message_unknown_error);
		GameSaving::get().save();
	}

	system("pause");
	return EXIT_FAILURE;
}