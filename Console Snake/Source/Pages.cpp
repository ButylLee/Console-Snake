#include "Pages.h"
#include "Resource.h"
#include "KeyMap.h"

#include "wideIO.h"
#include "ScopeGuard.h"

#include "DataSet.h"
#include "Playground.h"
#include "Rank.h"
#include "GameSaving.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <memory>
#include <string>

/***************************************
 class GamePage
****************************************/
void GamePage::run()
{
	auto sg_title = sg::MakeScopeGuard([] { Console::get().setTitle(~token::console_title); });
	if (GameSetting::get().show_frame)
	{
		Console::get().setTitle(~token::title_gaming);
	}
	else
	{
		Console::get().setConsoleWindow(Console::NoFrame);
		sg_title.dismiss();
	}
	auto width = GameSetting::get().width;
	auto height = GameSetting::get().height;
	canvas.setClientSize(width, height);

	std::atomic<Direction> input_key = Direction::None;
	std::atomic<Direction> snake_direct;
	std::atomic<GameStatus> game_status;

	std::mutex mut;
	std::condition_variable cond_ready;
	std::thread th_input(
		[&]
		{
			std::unique_lock lk(mut);
			cond_ready.wait(lk, [] { return true; }); // wait for playground initializing
			while (true)
			{
				if (input_key != Direction::None)
					continue;
				switch (getwch())
				{
					case K_UP: case K_W: case K_w:
						if (game_status == GameStatus::Running && snake_direct != Direction::Down)
							input_key = Direction::Up;
						break;

					case K_DOWN: case K_S: case K_s:
						if (game_status == GameStatus::Running && snake_direct != Direction::Up)
							input_key = Direction::Down;
						break;

					case K_LEFT: case K_A: case K_a:
						if (game_status == GameStatus::Running && snake_direct != Direction::Right)
							input_key = Direction::Left;
						break;

					case K_RIGHT: case K_D: case K_d:
						if (game_status == GameStatus::Running && snake_direct != Direction::Left)
							input_key = Direction::Right;
						break;

					case K_Space:
						if (game_status == GameStatus::Pausing)
						{
							game_status = GameStatus::Running;
							if (GameSetting::get().show_frame)
								Console::get().setTitle(~token::title_gaming);
						}
						else
						{
							game_status = GameStatus::Pausing;
							if (GameSetting::get().show_frame)
								Console::get().setTitle(~token::title_pausing);
						}
						break;

					case K_Esc:
						game_status = GameStatus::Ending;
						return;
				}
			}
		});
	ON_SCOPE_EXIT{
		if (th_input.joinable())
			th_input.join();
	};

	{
		std::unique_lock lk(mut);
		Playground playground(this->canvas,
							  input_key,
							  snake_direct,
							  game_status);
		lk.unlock();
		cond_ready.notify_one();

		playground.play();
	}
	if (GameData::get().retry_game == true)
	{
		GameData::get().retry_game = false;
	}
	else
	{
		GameData::get().seletion = PageSel::MenuPage;
	}
	GameSaving::get().save();
}

/***************************************
 class AboutPage
****************************************/
void AboutPage::run()
{
	int msg = MessageBoxW(Console::get().getConsoleHandle(),
						  (~token::about_text).c_str(),
						  (~token::about_caption).c_str(),
						  MB_OK | MB_ICONINFORMATION);
	if (msg != IDOK)
		throw NativeError(GetLastError());
	GameData::get().seletion = PageSel::MenuPage;
}

/***************************************
 Base class MainPage
****************************************/
void MainPage::paintTitle(ShowVersion show_version)
{
	canvas.setColor(Color::LightBlue);
	print(game_title);
	if (show_version == ShowVersion::Yes)
		print(~token::game_version);
}

/***************************************
 class MenuPage
****************************************/
void MenuPage::run()
{
	Console::get().setConsoleWindow(Console::UseFrame);
	canvas.setClientSize(default_size);
	paintInterface();

	while (true)
	{
		switch (getwch())
		{
			case K_Enter:
			case K_1:
				GameData::get().seletion = PageSel::GamePage;
				return;
			case K_2:
				GameData::get().seletion = PageSel::SettingPage;
				return;
			case K_3:
				GameData::get().seletion = PageSel::RankPage;
				return;
			case K_a:
			case K_A:
			case K_F1:
				GameData::get().seletion = PageSel::AboutPage;
				return;
			case K_Esc:
				exit(EXIT_SUCCESS);
		}
	}
}

void MenuPage::paintInterface()
{
	paintTitle(ShowVersion::Yes);

	auto [baseX, baseY] = canvas.getClientSize();
	baseX = baseX / 3 + 4;
	baseY = baseY / 2 + 2;
	canvas.setColor(Color::LightWhite);

	canvas.setCursor(baseX, baseY);
	print(~token::menu_start_game);
	canvas.setCursor(baseX, baseY + 2);
	print(~token::menu_setting);
	canvas.setCursor(baseX, baseY + 4);
	print(~token::menu_rank);
	canvas.setCursor(baseX - 1, baseY + 6);
	print(~token::menu_exit);
}

/***************************************
 class SettingPage
****************************************/
void SettingPage::run()
{
	canvas.setClientSize(default_size);
	paintInterface();

	while (true)
	{
		paintCurOptions();
		switch (getwch())
		{
			case K_1:
			{
				GameSetting::get().speed.nextValue();
			}
			break;

			case K_2:
			{
				GameSetting::get().width.nextValue();
				GameSetting::get().height.nextValue();
			}
			break;

			case K_3:
			{
				GameSetting::get().show_frame = !GameSetting::get().show_frame;
			}
			break;

			case K_4:
			{
				LocalizedStrings::setLang(
					GameSetting::get().lang.nextValue()
				);
				Console::get().setTitle(~token::console_title);
				canvas.clear();
				paintInterface();
			}
			break;

			case K_Enter:
			{
				GameData::get().seletion = PageSel::MenuPage;
				GameSaving::get().save();
			}
			return;

			case K_Esc:
			{
				GameData::get().seletion = PageSel::MenuPage;
				// restore
				GameSetting::get() = setting_backup;
				LocalizedStrings::setLang(setting_backup.lang);
				Console::get().setTitle(~token::console_title);
			}
			return;
		}
	}
}

void SettingPage::paintInterface()
{
	paintTitle(ShowVersion::No);

	auto [baseX, baseY] = canvas.getClientSize();
	baseX = baseX / 3 + 1;
	baseY = baseY / 2 + 1;
	canvas.setColor(Color::White);

	canvas.setCursor(baseX, baseY);
	print(~token::setting_speed);
	canvas.setCursor(baseX, baseY + 2);
	print(~token::setting_map_size);
	canvas.setCursor(baseX, baseY + 4);
	print(~token::setting_show_frame);
	canvas.setCursor(baseX, baseY + 6);
	print(~token::setting_language);
	canvas.setCursor(baseX - 2, baseY + 8);
	print(~token::setting_save);
	canvas.setCursor(baseX - 1, baseY + 10);
	print(~token::setting_return);
}

void SettingPage::paintCurOptions()
{
	auto [baseX, baseY] = canvas.getClientSize();
	baseX = baseX / 2 + 3;
	baseY = baseY / 2 + 1;
	canvas.setColor(Color::White);

	canvas.setCursor(baseX, baseY);
	print(~GameSetting::get().speed.Name());

	canvas.setCursor(baseX, baseY + 2);
	print(L"%hd X %hd"_crypt, GameSetting::get().width.Value(), GameSetting::get().height.Value());
	print(GameSetting::get().width.Name());

	canvas.setCursor(baseX, baseY + 4);
	print(GameSetting::get().show_frame
		  ? ~token::setting_show_frame_yes
		  : ~token::setting_show_frame_no);

	canvas.setCursor(baseX, baseY + 6);
	print(GameSetting::get().lang.Name());
}

/***************************************
 class BeginPage
****************************************/
// benign data race
bool BeginPage::is_press = false;

void BeginPage::run()
{
	canvas.setClientSize(default_size);
	paintInterface();

	if (getwch())
	{
		is_press = true;
		GameData::get().seletion = PageSel::MenuPage;
		return;
	}
}

void BeginPage::paintInterface()
{
	auto [baseX, baseY] = canvas.getClientSize();
	canvas.setColor(Color::LightWhite);
	canvas.setCenteredCursor(~token::press_any_key, baseY / 2 + 4);
	print(~token::press_any_key);

	std::thread th_paint(
		[this]
		{
			using namespace std::chrono_literals;
			for (bool color_flag = false;;)
			{
				if (is_press)
					return;
				canvas.setCursor(0, 0);
				canvas.setColor(color_flag ? Color::Aqua : Color::LightBlue);
				print(game_title);
				print(~token::game_version);
				color_flag = !color_flag;
				std::this_thread::sleep_for(900ms);
			}
		});
	th_paint.detach();
}

/***************************************
 class RankPage
****************************************/
void RankPage::run()
{
	canvas.setClientSize(default_size);
	paintInterface();

	while (true)
	{
		switch (getwch())
		{
			case K_Ctrl_Dd:
				Rank::get().clearRank();
				GameSaving::get().save();
				[[fallthrough]];
			case K_Enter:
			case K_Esc:
				GameData::get().seletion = PageSel::MenuPage;
				return;
		}
	}
}

void RankPage::paintInterface()
{
	using namespace std::chrono_literals;
	paintTitle(ShowVersion::No);

	auto [baseX, baseY] = canvas.getClientSize();
	baseX = baseX / 4 - 1;
	baseY = baseY / 2 + 1;
	canvas.setColor(Color::LightYellow);

	if (Rank::get().getRank()[0].score == 0)
	{
		canvas.setCenteredCursor(~token::rank_no_data, baseY);
		print(~token::rank_no_data);
	}
	else
	{
		int number = 0;
		std::wstring buffer, name, score, speed;
		for (const auto& item : Rank::get().getRank())
		{
			if (item.score == 0)
				break;
			std::this_thread::sleep_for(50ms);

			// no std::format, sad:-(
			buffer = ~token::rank_No; // arg:number
			buffer += L"%-*ls"_crypt; // arg:name width, name.c_str
			buffer += item.is_win ? L" %ls"_crypt : L" %4ls"_crypt; // arg:score or win
			buffer += L" | "_crypt;
			buffer += ~token::rank_setting;
			buffer += L"%-*ls "_crypt; // arg:speed setting width, speed setting
			buffer += L"%2d X %2d"_crypt; // arg:size setting
			canvas.setCursor(baseX, baseY + number);
			name = item.name.empty() ? ~token::rank_anonymous : item.name;
			score = item.is_win ? ~token::rank_win : std::to_wstring(item.score);
			speed = ~Speed::getNameFrom(item.speed);

			print(buffer, ++number,
				  Rank::name_max_length - StrFullWidthCount(name), name.c_str(),
				  score.c_str(),
				  6 - StrFullWidthCount(speed), speed.c_str(),
				  item.width, item.height);
		}

		std::this_thread::sleep_for(50ms);
		canvas.setColor(Color::White);
		canvas.setCursor(baseX / 2, baseY + 12);
		print(~token::rank_clear_all_records);
	}
	std::this_thread::sleep_for(500ms);
}

/***************************************
 Function CreatePage
****************************************/
std::unique_ptr<Page> CreatePage()
{
	std::unique_ptr<Page> page;

	switch (GameData::get().seletion)
	{
		case PageSel::BeginPage:
			page.reset(new BeginPage);
			break;
		case PageSel::MenuPage:
			page.reset(new MenuPage);
			break;
		case PageSel::GamePage:
			page.reset(new GamePage);
			break;
		case PageSel::SettingPage:
			page.reset(new SettingPage);
			break;
		case PageSel::RankPage:
			page.reset(new RankPage);
			break;
		case PageSel::AboutPage:
			page.reset(new AboutPage);
			break;
	}
	return page;
}