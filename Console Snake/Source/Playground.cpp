#include "PlayGround.h"
#include "Rank.h"
#include "Console.h"
#include "GlobalData.h"
#include "Resource.h"
#include "WideIO.h"
#include "KeyMap.h"
#include "ScopeGuard.h"
#include "Timer.h"

#include <thread>
#include <atomic>
#include <chrono>
#include <string>
#include <memory>
#include <cwctype>
#include "WinHeader.h"

PlayGround::PlayGround(Canvas& canvas) :canvas(canvas), arena(canvas)
{
	GameData::get().score = 0;
	game_status = GameStatus::Running;
}

void PlayGround::play()
{
	using namespace std::chrono_literals;

	std::thread th_input(
		[this]
		{
			while (true)
			{
				if (arena.isOver())
					return;
				if (arena.input_key != Direction::None)
					continue;
				auto ch = getwch();
				if (game_status == GameStatus::Running)
					switch (ch)
					{
						case K_UP: case K_W: case K_w:
							arena.input_key = +Direction::Up;
							break;

						case K_DOWN: case K_S: case K_s:
							arena.input_key = +Direction::Down;
							break;

						case K_LEFT: case K_A: case K_a:
							arena.input_key = +Direction::Left;
							break;

						case K_RIGHT: case K_D: case K_d:
							arena.input_key = +Direction::Right;
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

	std::atomic<bool> pause_flicker_flag = false;
	Timer timer([&]
				{
					pause_flicker_flag = !pause_flicker_flag;
				}, pause_flicker_interval, Timer::Loop);

	while (true)
	{
		switch (game_status)
		{
			case GameStatus::Running:
			{
				arena.updateFrame();
				if (arena.isOver())
				{
					// tell thread th_input to end
					ungetwch(K_Esc);
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
				if (pause_flicker_flag)
					arena.paintElement(Element::Snake, x, y);
				else
					arena.paintElement(arena.getPositionType(x, y), x, y);
				std::this_thread::sleep_for(1us);
			}
			break;

			case GameStatus::Ending:
				return;
		}
	}
}

void PlayGround::ending()
{
	auto [baseX, baseY] = canvas.getClientSize();
	baseY = baseY / 2 - 5;
	std::wstring buffer;

	// show game over info
	if (arena.isWin())
	{
		Console::get().setTitle(~Token::game_congratulations);
		canvas.setColor(Color::Green);

		canvas.setCursorCentered(~Token::game_you_win, baseY);
		print(~Token::game_you_win);
	}
	else
	{
		canvas.setColor(Color::LightWhite);

		canvas.setCursorCentered(~Token::game_you_died, baseY);
		print(~Token::game_you_died);
	}
	buffer = ~Token::game_show_score;
	buffer += std::to_wstring(GameData::get().score);
	canvas.setCursorCentered(buffer, baseY + 1);
	print(buffer);

	// show info and get gamer's name
	if (GameData::get().score != 0)
	{
		canvas.setColor(Color::Green);
		canvas.setCursorCentered(~Token::game_enter_your_name, baseY + 3);
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
		Rank::get().newResult(name, GameData::get().score, arena.isWin());
	}

	// show Retry Or Return info
	canvas.setColor(Color::LightWhite);
	canvas.setCursorCentered(~Token::game_Space_to_retry, baseY + 6);
	print(~Token::game_Space_to_retry);

	canvas.setCursorCentered(~Token::game_Esc_to_return, baseY + 7);
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