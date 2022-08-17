#include "Pages.h"
#include "Console.h"
#include "Playground.h"
#include "Rank.h"
#include "GameSaving.h"

#include "wideIO.h"
#include "ScopeGuard.h"

#include "Resource.h"
#include "KeyMap.h"
#include "GlobalData.h"
#include "ErrorHandling.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "AtomicOperation.h"
#include <chrono>
#include <memory>
#include <string>
#include <cassert>

/***************************************
 Interface Page
****************************************/
std::unique_ptr<Page> Page::Create()
{
	auto& map = Page::FUNC_GET_MAP();
	auto& key = GameData::get().selection;
	assert(map.find(key) != map.end());
	return map[key]();
}

/***************************************
 class GamePage
****************************************/
void GamePage::run()
{
	auto sg_title = sg::MakeScopeGuard([] { Console::get().setTitle(~Token::console_title); });
	if (GameSetting::get().show_frame)
	{
		Console::get().setTitle(~Token::title_gaming);
	}
	else
	{
		Console::get().setConsoleWindow(Console::NoFrame);
		sg_title.dismiss();
	}
	auto width = GameSetting::get().width;
	auto height = GameSetting::get().height;
	canvas.setClientSize(width, height);

	Playground playground(this->canvas);
	playground.play();

	if (GameData::get().retry_game == true)
	{
		GameData::get().retry_game = false;
	}
	else
	{
		GameData::get().selection = PageSelect::MenuPage;
	}
	GameSaving::get().save();
}

/***************************************
 class AboutPage
****************************************/
void AboutPage::run()
{
	int msg = MessageBoxW(Console::get().getConsoleHandle(),
						  (~Token::about_text).c_str(),
						  (~Token::about_caption).c_str(),
						  MB_OK | MB_ICONINFORMATION);
	if (msg != IDOK)
		throw NativeException{};
	GameData::get().selection = PageSelect::MenuPage;
}

/***************************************
 Base class NormalPage
****************************************/
void NormalPage::paintTitle(ShowVersion show_version)
{
	canvas.setColor(Color::LightBlue);
	print(game_title);
	if (show_version == ShowVersion::Yes)
		print(~Token::game_version);
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
				GameData::get().selection = PageSelect::GamePage;
				return;
			case K_2:
				GameData::get().selection = PageSelect::SettingPage;
				return;
			case K_3:
				GameData::get().selection = PageSelect::RankPage;
				return;
			case K_a:
			case K_A:
			case K_F1:
				GameData::get().selection = PageSelect::AboutPage;
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
	print(~Token::menu_start_game);
	canvas.setCursor(baseX, baseY + 2);
	print(~Token::menu_setting);
	canvas.setCursor(baseX, baseY + 4);
	print(~Token::menu_rank);
	canvas.setCursor(baseX - 1, baseY + 6);
	print(~Token::menu_exit);
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
				GameData::get().selection = PageSelect::CustomThemePage;
				auto page = Page::Create();
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
				Console::get().setTitle(~Token::console_title);
				canvas.clear();
				paintInterface();
			}
			break;

			case K_Enter:
			{
				GameData::get().selection = PageSelect::MenuPage;
				GameSaving::get().save();
			}
			return;

			case K_Esc:
			{
				GameData::get().selection = PageSelect::MenuPage;
				// restore
				GameSetting::get() = setting_backup;
				LocalizedStrings::setLang(setting_backup.lang);
				if (custom_theme_backup)
					GameSetting::get().theme.setCustomValue(*custom_theme_backup);
				else
					GameSetting::get().theme.clearCustomValue();
				Console::get().setTitle(~Token::console_title);
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
	print(~Token::setting_speed);
	canvas.setCursor(baseX, baseY + 2);
	print(~Token::setting_map_size);
	canvas.setCursor(baseX, baseY + 4);
	print(~Token::setting_show_frame);
	canvas.setCursor(baseX, baseY + 6);
	print(~Token::setting_theme);
	canvas.setCursor(baseX - 9, baseY + 6);
	print(~Token::setting_customize_theme);
	canvas.setCursor(baseX, baseY + 8);
	print(~Token::setting_language);
	canvas.setCursor(baseX - 2, baseY + 10);
	print(~Token::setting_save);
	canvas.setCursor(baseX - 1, baseY + 12);
	print(~Token::setting_return);
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
	print(L"%hd X %hd", GameSetting::get().width.Value(), GameSetting::get().height.Value());
	print(GameSetting::get().width.Name());

	canvas.setCursor(baseX, baseY + 4);
	print(GameSetting::get().show_frame
		  ? ~Token::setting_show_frame_yes
		  : ~Token::setting_show_frame_no);

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
				GameData::get().selection = PageSelect::SettingPage;
				return;

			case K_Enter:
			case K_Esc:
				GameSetting::get().theme.setCustomValue(theme_temp);
				GameSetting::get().theme = Theme::Custom;
				GameData::get().selection = PageSelect::SettingPage;
				return;
		}
	}
}

void CustomThemePage::paintInterface()
{
	static constexpr auto custom_theme_title = LR"title(
            ______           __                     ________                      
           / ____/_  _______/ /_____  ____ ___     /_  __/ /_  ___  ____ ___  ___ 
          / /   / / / / ___/ __/ __ \/ __ `__ \     / / / __ \/ _ \/ __ `__ \/ _ \
         / /___/ /_/ (__  ) /_/ /_/ / / / / / /    / / / / / /  __/ / / / / /  __/
         \____/\__,_/____/\__/\____/_/ /_/ /_/    /_/ /_/ /_/\___/_/ /_/ /_/\___/ 
                                                                                  )title";
	canvas.setColor(Color::LightAqua);
	print(custom_theme_title);

	canvas.setColor(Color::White);
	canvas.setCursor(5, 30);
	print(~Token::setting_reset_custom);

	canvas.setCursor(30, 14);
	print(~Token::custom_theme_list_head);
	canvas.setCursor(22, 16);
	print(~Token::custom_theme_blank);
	print(L"(A)             (Q)");
	canvas.setCursor(22, 18);
	print(~Token::custom_theme_food);
	print(L"(S)             (W)");
	canvas.setCursor(22, 20);
	print(~Token::custom_theme_snake);
	print(L"(D)             (E)");
	canvas.setCursor(22, 22);
	print(~Token::custom_theme_barrier);
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
void BeginPage::run()
{
	canvas.setClientSize(default_size);
	paintInterface();

	(void)getwch();
	is_press = true;
	GameData::get().selection = PageSelect::MenuPage;
}

void BeginPage::paintInterface()
{
	auto [baseX, baseY] = canvas.getClientSize();
	canvas.setColor(Color::LightWhite);
	canvas.setCenteredCursor(~Token::press_any_key, baseY / 2 + 4);
	print(~Token::press_any_key);

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
					print(~Token::game_version);
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
					print(~Token::game_version);
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
				if (is_no_data)
					continue;
				Rank::get().clearRank();
				GameSaving::get().save();
				[[fallthrough]];
			case K_Enter:
			case K_Esc:
				GameData::get().selection = PageSelect::MenuPage;
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
		canvas.setCenteredCursor(~Token::rank_no_data, baseY);
		print(~Token::rank_no_data);
		is_no_data = true;
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
			buffer = ~Token::rank_No; // arg:number
			buffer += L"%-*ls"; // arg:name width, name.c_str
			buffer += item.is_win ? L" %ls" : L" %4ls"; // arg:score or win
			buffer += L" | ";
			buffer += ~Token::rank_setting;
			buffer += L"%-*ls "; // arg:speed setting width, speed setting
			buffer += L"%2d X %2d"; // arg:size setting
			canvas.setCursor(baseX, baseY + number);
			name = item.name.empty() ? ~Token::rank_anonymous : item.name;
			score = item.is_win ? ~Token::rank_win : std::to_wstring(item.score);
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
		print(~Token::rank_clear_all_records);
	}
	std::this_thread::sleep_for(500ms);
}