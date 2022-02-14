#include "Pages.h"
#include "Console.h"
#include "Playground.h"
#include "Rank.h"
#include "GameSaving.h"

#include "wideIO.h"
#include "ScopeGuard.h"

#include "Resource.h"
#include "KeyMap.h"
#include "DataSet.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "AtomicOperation.h"
#include <chrono>
#include <memory>
#include <string>

/***************************************
 Interface Page
****************************************/
std::unique_ptr<Page> Page::CreatePage()
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
		case PageSel::CustomThemePage:
			page.reset(new CustomThemePage);
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
				if (loadAtomic(input_key) != Direction::None)
					continue;
				switch (getwch())
				{
					case K_UP: case K_W: case K_w:
						if (loadAtomic(game_status) == GameStatus::Running &&
							loadAtomic(snake_direct) != Direction::Down)
						{
							storeAtomic(input_key, Direction::Up);
						}
						break;

					case K_DOWN: case K_S: case K_s:
						if (loadAtomic(game_status) == GameStatus::Running &&
							loadAtomic(snake_direct) != Direction::Up)
						{
							storeAtomic(input_key, Direction::Down);
						}
						break;

					case K_LEFT: case K_A: case K_a:
						if (loadAtomic(game_status) == GameStatus::Running &&
							loadAtomic(snake_direct) != Direction::Right)
						{
							storeAtomic(input_key, Direction::Left);
						}
						break;

					case K_RIGHT: case K_D: case K_d:
						if (loadAtomic(game_status) == GameStatus::Running &&
							loadAtomic(snake_direct) != Direction::Left)
						{
							storeAtomic(input_key, Direction::Right);
						}
						break;

					case K_Space:
						if (loadAtomic(game_status) == GameStatus::Pausing)
						{
							storeAtomic(game_status, GameStatus::Running);
							if (GameSetting::get().show_frame)
								Console::get().setTitle(~token::title_gaming);
						}
						else
						{
							storeAtomic(game_status, GameStatus::Pausing);
							if (GameSetting::get().show_frame)
								Console::get().setTitle(~token::title_pausing);
						}
						break;

					case K_Esc:
						storeAtomic(game_status, GameStatus::Ending);
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
		throw NativeException{};
	GameData::get().seletion = PageSel::MenuPage;
}

/***************************************
 Base class NormalPage
****************************************/
void NormalPage::paintTitle(ShowVersion show_version)
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
				GameSetting::get().speed.setNextValue();
			}
			break;

			case K_2:
			{
				GameSetting::get().width.setNextValue();
				GameSetting::get().height.setNextValue();
			}
			break;

			case K_3:
			{
				GameSetting::get().show_frame = !GameSetting::get().show_frame;
			}
			break;

			case K_4:
			{
				GameSetting::get().theme.setNextValue();
			}
			break;

			case K_F4:
			{
				GameData::get().seletion = PageSel::CustomThemePage;
				auto page = CreatePage();
				page->run();
				canvas.setClientSize(default_size);
				paintInterface();
			}
			break;

			case K_5:
			{
				LocalizedStrings::setLang(
					GameSetting::get().lang.setNextValue()
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
				if (custom_theme_backup)
					GameSetting::get().theme.setCustomValue(*custom_theme_backup);
				else
					GameSetting::get().theme.clearCustomValue();
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
	print(~token::setting_theme);
	canvas.setCursor(baseX - 9, baseY + 6);
	print(~token::setting_customize_theme);
	canvas.setCursor(baseX, baseY + 8);
	print(~token::setting_language);
	canvas.setCursor(baseX - 2, baseY + 10);
	print(~token::setting_save);
	canvas.setCursor(baseX - 1, baseY + 12);
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
	print(~GameSetting::get().theme.Name());

	canvas.setCursor(baseX, baseY + 8);
	print(GameSetting::get().lang.Name());
}

/***************************************
 class CustomThemePage
****************************************/
CustomThemePage::CustomThemePage() noexcept
{
	auto custom_theme = GameSetting::get().theme.getCustomValue();
	if (custom_theme)
		theme_temp = *custom_theme;
	else
		theme_temp = Theme{ Theme::A };
}

void CustomThemePage::run()
{
	canvas.setClientSize(default_size);
	paintInterface();

	while (true)
	{
		paintCurOptions();
		switch (getwch())
		{
			case K_Q: case K_q:
				theme_temp[Element::blank].facade.setNextValue();
				break;
			case K_W: case K_w:
				theme_temp[Element::food].facade.setNextValue();
				break;
			case K_E: case K_e:
				theme_temp[Element::snake].facade.setNextValue();
				break;
			case K_R: case K_r:
				theme_temp[Element::barrier].facade.setNextValue();
				break;

			case K_A: case K_a:
				theme_temp[Element::blank].color.setNextValue();
				break;
			case K_S: case K_s:
				theme_temp[Element::food].color.setNextValue();
				break;
			case K_D: case K_d:
				theme_temp[Element::snake].color.setNextValue();
				break;
			case K_F: case K_f:
				theme_temp[Element::barrier].color.setNextValue();
				break;

			case K_Ctrl_Dd:
				GameSetting::get().theme.clearCustomValue();
				GameData::get().seletion = PageSel::SettingPage;
				return;

			case K_Enter:
			case K_Esc:
				GameSetting::get().theme.setCustomValue(theme_temp);
				GameSetting::get().theme = Theme::Custom;
				GameData::get().seletion = PageSel::SettingPage;
				return;
		}
	}
}

void CustomThemePage::paintInterface()
{
	static const auto custom_theme_title = LR"title(
            ______           __                     ________                      
           / ____/_  _______/ /_____  ____ ___     /_  __/ /_  ___  ____ ___  ___ 
          / /   / / / / ___/ __/ __ \/ __ `__ \     / / / __ \/ _ \/ __ `__ \/ _ \
         / /___/ /_/ (__  ) /_/ /_/ / / / / / /    / / / / / /  __/ / / / / /  __/
         \____/\__,_/____/\__/\____/_/ /_/ /_/    /_/ /_/ /_/\___/_/ /_/ /_/\___/ 
                                                                                  )title"_crypt;
	canvas.setColor(Color::LightAqua);
	print(custom_theme_title);

	canvas.setColor(Color::White);
	canvas.setCursor(5, 30);
	print(~token::setting_reset_custom);

	canvas.setCursor(30, 14);
	print(~token::custom_theme_list_head);
	canvas.setCursor(22, 16);
	print(~token::custom_theme_blank);
	print(L"(A)             (Q)");
	canvas.setCursor(22, 18);
	print(~token::custom_theme_food);
	print(L"(S)             (W)");
	canvas.setCursor(22, 20);
	print(~token::custom_theme_snake);
	print(L"(D)             (E)");
	canvas.setCursor(22, 22);
	print(~token::custom_theme_barrier);
	print(L"(F)             (R)");
}

void CustomThemePage::paintCurOptions()
{
	{
		constexpr int width = 16, height = 16;
		constexpr int origin_col = 5, origin_row = 11;

		int row = origin_row, column = origin_col;
		for (; row < origin_row + height; row++, column = origin_col)
		{
			canvas.setCursor(column, row);
			for (; column < origin_col + width; column++)
			{
				if (row == origin_row + 5 && column == origin_col + 3)
				{
					canvas.setColor(theme_temp[Element::snake].color);
					print(theme_temp[Element::snake].facade);
					print(theme_temp[Element::snake].facade);
					print(theme_temp[Element::snake].facade);
					column += 2;
				}
				else if (row == origin_row + 11 && column == origin_col + 10)
				{
					canvas.setColor(theme_temp[Element::food].color);
					print(theme_temp[Element::food].facade);
				}
				else if (row == origin_row || row == origin_row + height - 1 ||
						 column == origin_col || column == origin_col + width - 1)
				{
					canvas.setColor(theme_temp[Element::barrier].color);
					print(theme_temp[Element::barrier].facade);
				}
				else
				{
					canvas.setColor(theme_temp[Element::blank].color);
					print(theme_temp[Element::blank].facade);
				}
			}
		}
	}
	{
		constexpr int baseX = 29, baseY = 16;
		constexpr int nextX = baseX + 9;
		canvas.setColor(Color::White);

		canvas.setCursor(baseX, baseY);
		print(theme_temp[Element::blank].color.Name());
		canvas.setCursor(nextX, baseY);
		print(theme_temp[Element::blank].facade.Value());
		canvas.setCursor(baseX, baseY + 2);
		print(theme_temp[Element::food].color.Name());
		canvas.setCursor(nextX, baseY + 2);
		print(theme_temp[Element::food].facade.Value());
		canvas.setCursor(baseX, baseY + 4);
		print(theme_temp[Element::snake].color.Name());
		canvas.setCursor(nextX, baseY + 4);
		print(theme_temp[Element::snake].facade.Value());
		canvas.setCursor(baseX, baseY + 6);
		print(theme_temp[Element::barrier].color.Name());
		canvas.setCursor(nextX, baseY + 6);
		print(theme_temp[Element::barrier].facade.Value());
	}
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
			if (GameData::get().colorful_title)
				for (Color color;;)
				{
					if (is_press)
						return;
					canvas.setCursor(0, 0);
					canvas.setColor(color.setNextValue());
					print(game_title);
					print(~token::game_version);
					std::this_thread::sleep_for(100ms);
				}
			else
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

	if (auto [rank, lock] = Rank::get().getRank(); rank[0].score == 0)
	{
		canvas.setCenteredCursor(~token::rank_no_data, baseY);
		print(~token::rank_no_data);
	}
	else
	{
		int number = 0;
		std::wstring buffer, name, score, speed;
		for (const auto& item : rank)
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