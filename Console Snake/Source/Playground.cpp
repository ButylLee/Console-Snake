#include "Playground.h"
#include "Rank.h"
#include "Console.h"
#include "GlobalData.h"
#include "Resource.h"
#include "wideIO.h"
#include "KeyMap.h"
#include "ScopeGuard.h"

#include <thread>
#include <chrono>
#include <string>
#include <memory>
#include <cwctype>
#include "WinHeader.h"

Playground::Playground(Canvas& canvas) :canvas(canvas), arena(canvas)
{
	GameData::get().score = 0;
	game_status = GameStatus::Running;
}

void Playground::play()
{
	using namespace std::chrono_literals;

	std::thread th_input(
		[this]
		{
			while (true)
			{
				if (arena.is_over())
					return;
				if (arena.input_key != Direction::None)
					continue;
				auto ch = getwch();
				if (game_status == GameStatus::Running)
					switch (ch)
					{
						case K_UP: case K_W: case K_w:
							arena.input_key = Direction::Up;
							break;

						case K_DOWN: case K_S: case K_s:
							arena.input_key = Direction::Down;
							break;

						case K_LEFT: case K_A: case K_a:
							arena.input_key = Direction::Left;
							break;

						case K_RIGHT: case K_D: case K_d:
							arena.input_key = Direction::Right;
							break;
					}
				switch (ch)
				{
					case K_Space:
						if (game_status == GameStatus::Pausing)
						{
							game_status = GameStatus::Running;
							if (GameSetting::get().show_frame)
								Console::get().setTitle(~Token::title_gaming);
						}
						else
						{
							game_status = GameStatus::Pausing;
							if (GameSetting::get().show_frame)
								Console::get().setTitle(~Token::title_pausing);
						}
						break;

					case K_Esc:
						game_status = GameStatus::Ending;
						return;
				}
			}
		});
	finally {
		if (th_input.joinable())
			th_input.join();
	};

	bool pause_flicker_flag = false;
	auto timer_end = std::make_shared<bool>(false);

	std::thread th_timer(
		[timer_end, &pause_flicker_flag]
		{
			using namespace std::chrono_literals;
			for (; !*timer_end;)
			{
				std::this_thread::sleep_for(pause_flicker_interval);
				pause_flicker_flag = !pause_flicker_flag;
			}
		});
	th_timer.detach();
	finally {
		*timer_end = true;
	};

	while (true)
	{
		switch (game_status)
		{
			case GameStatus::Running:
			{
				arena.updateFrame();
				if (arena.is_over())
				{
					// tell thread th_input to end
					PostMessageW(Console::get().getConsoleHandle(), WM_KEYDOWN, VK_ESCAPE, 0);
					ending();
					return;
				}
				else
				{
					std::this_thread::sleep_for(30ms + // 30ms - 210ms, level 1-10
												20ms * (10 - GameSetting::get().speed.Value()));
				}
			}
			break;

			case GameStatus::Pausing:
			{
				auto [x, y] = arena.getNextPosition();
				canvas.setCursor(x, y);
				if (pause_flicker_flag)
					arena.paintElement(Element::snake);
				else
					arena.paintElement(arena.getPositionType(x, y));
			}
			break;

			case GameStatus::Ending:
				return;
		}
	}
}

void Playground::ending()
{
	auto [baseX, baseY] = canvas.getClientSize();
	baseY = baseY / 2 - 5;
	std::wstring buffer;

	// show gameover info
	if (arena.is_win())
	{
		Console::get().setTitle(~Token::game_congratulations);
		canvas.setColor(Color::Green);

		canvas.setCenteredCursor(~Token::game_you_win, baseY);
		print(~Token::game_you_win);
	}
	else
	{
		canvas.setColor(Color::LightWhite);

		canvas.setCenteredCursor(~Token::game_you_died, baseY);
		print(~Token::game_you_died);
	}
	buffer = ~Token::game_show_score;
	buffer += std::to_wstring(GameData::get().score);
	canvas.setCenteredCursor(buffer, baseY + 1);
	print(buffer);

	// show info and get gamer's name
	if (GameData::get().score != 0)
	{
		canvas.setColor(Color::Green);
		canvas.setCenteredCursor(~Token::game_enter_your_name, baseY + 3);
		print(~Token::game_enter_your_name);

		canvas.setColor(Color::LightAqua);
		std::wstring name;
		for (wchar_t ch;;)
		{
			ch = getwchar();
			if (ch == L'\n')
				break;
			if (iswprint(ch))
				name += ch;
		}
		if (name.find_first_not_of(L' ') == std::string::npos)
			name.clear(); // clear if only has spaces
		if (name.length() > Rank::name_max_length)
			name.resize(Rank::name_max_length);
		Rank::get().newResult(name, GameData::get().score, arena.is_win());
	}

	// show Retry Or Return info
	canvas.setColor(Color::LightWhite);
	canvas.setCenteredCursor(~Token::game_Space_to_retry, baseY + 6);
	print(~Token::game_Space_to_retry);

	canvas.setCenteredCursor(~Token::game_Esc_to_return, baseY + 7);
	print(~Token::game_Esc_to_return);

	while (true)
	{
		switch (getwch())
		{
			case K_Space:
				GameData::get().retry_game = true;
				[[fallthrough]];
			case K_Esc:
				return;
		}
	}
}