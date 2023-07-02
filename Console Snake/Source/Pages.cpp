#include "PageInterface.h"
#include "Pages.h"
#include "Console.h"
#include "PlayGround.h"
#include "DemoGround.h"
#include "Rank.h"
#include "GameSaving.h"

#include "WideIO.h"
#include "Timer.h"
#include "Random.h"
#include "ScopeGuard.h"
#include "EncryptedString.h"
#include "Resource.h"
#include "KeyMap.h"
#include "GlobalData.h"
#include "ErrorHandling.h"

#include <thread>
#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>
#include "WinHeader.h"

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
	Console::get().setTitle(~Token::title_gaming);
	if (GameSetting::get().show_frame)
		Console::get().setConsoleWindow(Console::UseFrame);
	else
		Console::get().setConsoleWindow(Console::NoFrame);
	auto width = GameSetting::get().map.size.Value();
	auto height = GameSetting::get().map.size.Value();
	canvas.setClientSize(width, height);

	PlayGround playground(this->canvas);
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
 class DemoPage
****************************************/
void DemoPage::run()
{
	using namespace std::chrono_literals;
	Console::get().setTitle(~Token::press_any_key);
	Timer timer([]
				{
					static bool flicker = true;
					if (flicker = !flicker)
						Console::get().setTitle(~Token::press_any_key);
					else
						Console::get().setTitle({});
				}, 800ms, Timer::Loop,
				[]
				{
					Console::get().setTitle(~Token::console_title);
				});
	canvas.clear();
	auto [width2, height2] = canvas.getClientSize();
	auto width1 = GameSetting::get().map.size.Value();
	auto height1 = GameSetting::get().map.size.Value();
	canvas.setCursorOffset((width2 - width1) / 2, (height2 - height1) / 2);

	DemoGround demoground(this->canvas);
	demoground.show();

	GameData::get().selection = PageSelect::BeginPage;
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
 abstract class NormalPage
****************************************/
void NormalPage::paintTitle(ShowVersion show_version)
{
	canvas.setColor(Color::LightBlue);
	print(Resource::game_title);
	if (show_version == ShowVersion::Yes)
		print(~Token::game_version);
}

/***************************************
 class MenuPage
****************************************/
void MenuPage::run()
{
	Console::get().setTitle(~Token::console_title);
	Console::get().setConsoleWindow(Console::UseFrame);
	canvas.setClientSize(default_size);
	paintInterface();

	using namespace std::chrono_literals;
	std::atomic<bool> enter_demoground = false;
	Timer timer([&]
				{
					enter_demoground = true;
					ungetwch(K_Ctrl_Home);
				}, 15s);

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
			case K_Ctrl_Home:
				if (enter_demoground)
					GameData::get().selection = PageSelect::DemoPage;
				else
					GameData::get().selection = PageSelect::BeginPage;
				return;
			case K_Esc:
				GameData::get().exit_game = true;
				return;
		}
		timer.reset(); // press any key
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
				GameSetting::get().map.setNextValue();
			}
			break;

			case K_F2:
			{
				GameData::get().selection = PageSelect::CustomMapPage;
				auto page = Page::Create();
				page->run();
				canvas.setClientSize(default_size);
				paintInterface();
			}
			break;

			case K_3:
			{
				if (!GameSetting::get().old_console_host)
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
					GameSetting::get().lang.setNextValue().Value()
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
				LocalizedStrings::setLang(setting_backup.lang.Value());
				if (custom_theme_backup)
					GameSetting::get().theme.SetCustomValue(*custom_theme_backup);
				else
					GameSetting::get().theme.ClearCustomValue();
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
	canvas.setCursor(baseX - 9, baseY + 2);
	print(~Token::setting_customize_map);
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
	print(GameSetting::get().map.set.Name());
	print(L" - ");
	print(GameSetting::get().map.size.Name());

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
	auto custom_theme = GameSetting::get().theme.GetCustomValue();
	if (custom_theme)
		theme_temp = *custom_theme;
	else
		generateRandomTheme();
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
				theme_temp[Element::Blank].facade.setNextValue();
				break;
			case K_W: case K_w:
				theme_temp[Element::Food].facade.setNextValue();
				break;
			case K_E: case K_e:
				theme_temp[Element::Snake].facade.setNextValue();
				break;
			case K_R: case K_r:
				theme_temp[Element::Barrier].facade.setNextValue();
				break;

			case K_A: case K_a:
				theme_temp[Element::Blank].color.setNextValue();
				break;
			case K_S: case K_s:
				theme_temp[Element::Food].color.setNextValue();
				break;
			case K_D: case K_d:
				theme_temp[Element::Snake].color.setNextValue();
				break;
			case K_F: case K_f:
				theme_temp[Element::Barrier].color.setNextValue();
				break;

			case K_Ctrl_Dd:
				GameSetting::get().theme.ClearCustomValue();
				GameData::get().selection = PageSelect::SettingPage;
				return;

			case K_Enter:
				GameSetting::get().theme = Theme::Custom;
				[[fallthrough]];
			case K_Esc:
				GameSetting::get().theme.SetCustomValue(theme_temp);
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
                                                                                  )title"; // Slant
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
					canvas.setColor(theme_temp[Element::Snake].color);
					print(theme_temp[Element::Snake].facade.Value());
					print(theme_temp[Element::Snake].facade.Value());
					print(theme_temp[Element::Snake].facade.Value());
					column += 2;
				}
				else if (row == origin_row + 11 && column == origin_col + 10)
				{
					canvas.setColor(theme_temp[Element::Food].color);
					print(theme_temp[Element::Food].facade.Value());
				}
				else if (row == origin_row || row == origin_row + height - 1 ||
						 column == origin_col || column == origin_col + width - 1)
				{
					canvas.setColor(theme_temp[Element::Barrier].color);
					print(theme_temp[Element::Barrier].facade.Value());
				}
				else
				{
					canvas.setColor(theme_temp[Element::Blank].color);
					print(theme_temp[Element::Blank].facade.Value());
				}
			}
		}
	}
	{
		constexpr int baseX = 29, baseY = 16;
		constexpr int nextX = baseX + 9;
		canvas.setColor(Color::White);

		canvas.setCursor(baseX, baseY);
		print(theme_temp[Element::Blank].color.Name());
		canvas.setCursor(nextX, baseY);
		print(theme_temp[Element::Blank].facade.Value());
		canvas.setCursor(baseX, baseY + 2);
		print(theme_temp[Element::Food].color.Name());
		canvas.setCursor(nextX, baseY + 2);
		print(theme_temp[Element::Food].facade.Value());
		canvas.setCursor(baseX, baseY + 4);
		print(theme_temp[Element::Snake].color.Name());
		canvas.setCursor(nextX, baseY + 4);
		print(theme_temp[Element::Snake].facade.Value());
		canvas.setCursor(baseX, baseY + 6);
		print(theme_temp[Element::Barrier].color.Name());
		canvas.setCursor(nextX, baseY + 6);
		print(theme_temp[Element::Barrier].facade.Value());
	}
}

void CustomThemePage::generateRandomTheme()
{
	// generate distinct colors(no black)
	std::vector<int> color_candidate(Color::Mask_);
	std::iota(color_candidate.begin(), color_candidate.end(), 0);
	color_candidate.erase(std::find(color_candidate.cbegin(),
									color_candidate.cend(),
									static_cast<int>(Color::Black)));
	for (size_t i = 0; i < static_cast<size_t>(Element::Mask_); i++)
	{
		theme_temp[i].facade = static_cast<Facade::EnumTag>(GetRandom(0, static_cast<int>(Facade::Mask_) - 1));
		size_t index = GetRandom(0, color_candidate.size() - 1);
		theme_temp[i].color = static_cast<Color::EnumTag>(color_candidate[index]);
		color_candidate.erase(color_candidate.cbegin() + index);
	}
}

/***************************************
 class CustomMapPage
****************************************/
void CustomMapPage::MapSelector::paint()
{

}

void CustomMapPage::MapSelector::selectPrev()
{
}

void CustomMapPage::MapSelector::selectNext()
{
}

void CustomMapPage::MapSelector::deleteSelected()
{
}

void CustomMapPage::MapViewer::paint() const
{

}

void CustomMapPage::MapViewer::moveSelected(Direction direct)
{
	switch (direct)
	{
		case Direction::Up:
			y == 0 ? y = map.size.Value() - 1 : y--;
			break;
		case Direction::Down:
			y == map.size.Value() - 1 ? y = 0 : y++;
			break;
		case Direction::Left:
			x == 0 ? x = map.size.Value() - 1 : x--;
			break;
		case Direction::Right:
			x == map.size.Value() - 1 ? x = 0 : x++;
			break;
	}
	//TODO:paint curr and recover prev
}

void CustomMapPage::MapViewer::switchSelected()
{
}

void CustomMapPage::MapViewer::clearMap()
{
}

void CustomMapPage::run()
{
	canvas.setClientSize(default_size);
	paintInterface();
	canvas.setCursorOffset(0, 6);
	map_list.paint();
	canvas.setCursorOffset(0, 10);
	map_viewer.paint();
	canvas.setCursorOffset(0, 0);

	while (true)
	{
		paintCurOptions();
		switch (editor_level)
		{
			case EditorLevel::MapSelect:
				switch (getwch())
				{
					case K_F1:
						map_list.selectPrev(); break;
					case K_F2:
						map_list.selectNext(); break;
					case K_F3:
						editor_level = EditorLevel::MapEdit; break;
					case K_F4:
						map.size.setNextValue(); break;
					case K_Delete:
						//TODO:confirm delete
						map_list.deleteSelected(); break;
					case K_Enter: case K_Esc:
						GameData::get().selection = PageSelect::SettingPage;
						return;
				}
				break;
			case EditorLevel::MapEdit:
				switch (getwch())
				{
					case K_UP: case K_W: case K_w:
						map_viewer.moveSelected(MapViewer::Direction::Up); break;
					case K_DOWN: case K_S: case K_s:
						map_viewer.moveSelected(MapViewer::Direction::Down); break;
					case K_LEFT: case K_A: case K_a:
						map_viewer.moveSelected(MapViewer::Direction::Left); break;
					case K_RIGHT: case K_D: case K_d:
						map_viewer.moveSelected(MapViewer::Direction::Right); break;
					case K_Space:
						map_viewer.switchSelected(); break;
					case K_Ctrl_Bb:
						map_viewer.clearMap(); break;
					case K_Enter:
						editor_level = EditorLevel::MapSelect; break;
					case K_Esc:
						editor_level = EditorLevel::MapSelect; break;
				}
				break;
		}

	}
}

void CustomMapPage::paintInterface()
{
	static constexpr auto custom_map_title = LR"title(
                  ______           __                     __  ___          
                 / ____/_  _______/ /_____  ____ ___     /  |/  /___ _____ 
                / /   / / / / ___/ __/ __ \/ __ `__ \   / /|_/ / __ `/ __ \
               / /___/ /_/ (__  ) /_/ /_/ / / / / / /  / /  / / /_/ / /_/ /
               \____/\__,_/____/\__/\____/_/ /_/ /_/  /_/  /_/\__,_/ .___/ 
                                                                  /_/      )title" + 1; // Slant
	canvas.setColor(Color::LightAqua);
	print(custom_map_title);

	canvas.setColor(Color::White);
	canvas.setCursorOffset(25, 10);
	finally { canvas.setCursorOffset(0, 0); };

	canvas.setCursor(2, 0);
	print(~Token::custom_map_prev);
	canvas.setCursor(11, 0);
	print(~Token::custom_map_next);
	canvas.setCursor(2, 2);
	print(~Token::custom_map_edit_map);
	canvas.setCursor(11, 2);
	print(~Token::custom_map_delete_map);
	canvas.setCursor(2, 4);
	print(~Token::custom_map_switch_size);
	canvas.setCursor(1, 6);
	print(L"------------------------------------");

	canvas.setCursor(2, 11);
	print(~Token::custom_map_move_cursor);
	canvas.setCursor(2, 13);
	print(~Token::custom_map_switch_block);
	canvas.setCursor(2, 15);
	print(~Token::custom_map_all_blank);
	canvas.setCursor(2, 18);
	print(~Token::custom_map_save_edit);
	canvas.setCursor(2, 20);
	print(~Token::custom_map_cancel_edit);
}

void CustomMapPage::paintCurOptions()
{
	canvas.setColor(Color::White);
	canvas.setCursorOffset(25, 10);
	finally { canvas.setCursorOffset(0, 0); };

	canvas.setCursor(11, 4);
	print(~Token::custom_map_curr_size);
	print(map.size.Name());
	canvas.setCursor(2, 8);
	print(~Token::custom_map_curr_pos);

}

/***************************************
 class BeginPage
****************************************/
void BeginPage::run()
{
	canvas.setClientSize(default_size);
	paintInterface();

	(void)getwch();
	*is_press = true;
	GameData::get().selection = PageSelect::MenuPage;
}

void BeginPage::paintInterface()
{
	auto [baseX, baseY] = canvas.getClientSize();
	canvas.setColor(Color::LightWhite);
	canvas.setCursorCentered(~Token::press_any_key, baseY / 2 + 4);
	print(~Token::press_any_key);

	std::thread th_paint(
		[this, is_press = is_press]
		{
			using namespace std::chrono_literals;
			if (GameData::get().colorful_title)
				for (Color color;;)
				{
					if (*is_press)
						return;
					canvas.setCursor(0, 0);
					canvas.setColor(color.setNextValue());
					print(Resource::game_title);
					print(~Token::game_version);
					std::this_thread::sleep_for(200ms);
				}
			else
				for (bool color_flag = false;;)
				{
					if (*is_press)
						return;
					canvas.setCursor(0, 0);
					canvas.setColor(color_flag ? Color::Aqua : Color::LightBlue);
					print(Resource::game_title);
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
	canvas.setColor(Color::LightYellow);

	if (auto [rank, lock] = Rank::get().getRank(); rank[0].score == 0)
	{
		canvas.setCursorCentered(~Token::rank_no_data, baseY * 2 / 3);
		print(~Token::rank_no_data);
		is_no_data = true;
	}
	else
	{
		int number = 1;
		baseY = baseY / 2;
		std::wstring buffer, name, speed;
		for (const auto& item : rank)
		{
			if (item.score == 0)
				break;
			std::this_thread::sleep_for(50ms);

			canvas.setCursor(0, baseY + number);

			buffer = ::format(~Token::rank_No, number++);
			name = item.name.empty() ? ~Token::rank_anonymous : item.name;
			buffer += ::format(L"{:<{}}"_crypt, std::move(name), Rank::name_max_length);
			buffer += ::format(L" {:>4.4}"_crypt, item.is_win ? ~Token::rank_win : std::to_wstring(item.score));
			buffer += L" | "_crypt;
			buffer += ~Token::rank_setting;
			speed = ~Speed::GetNameFrom(item.speed);
			buffer += ::format(L"{:<{}} "_crypt, std::move(speed), 6);
			buffer += ::format(L"{:.{}} - {}"_crypt, item.map_name, Map::name_max_length, Size::GetNameFrom(item.size));

			buffer = ::format(L"{:^{}}"_crypt, std::move(buffer), baseX * 2);
			print(buffer);
		}

		std::this_thread::sleep_for(50ms);
		canvas.setColor(Color::White);
		canvas.setCursor(baseX / 9, baseY + 13);
		print(~Token::rank_clear_all_records);
	}
	std::this_thread::sleep_for(500ms);
}