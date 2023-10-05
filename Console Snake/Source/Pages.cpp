#include "PageInterface.h"
#include "Pages.h"
#include "Console.h"
#include "PlayGround.h"
#include "DemoGround.h"
#include "Rank.h"
#include "GameSaving.h"
#include "SoundPlayer.h"

#include "WideIO.h"
#include "Timer.h"
#include "Random.h"
#include "ErrorHandling.h"
#include "LocalizedStrings.h"
#include "ScopeGuard.h"
#include "EncryptedString.h"
#include "Pythonic.h"
#include "Resource.h"
#include "KeyMap.h"
#include "GlobalData.h"

#include <thread>
#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <numeric>
#include <algorithm>
#include <type_traits>
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
	Console::get().setTitle(GameSetting::get().opening_pause ? ~Token::title_pausing : ~Token::title_gaming);
	Console::get().setConsoleWindow(GameSetting::get().show_frame ? Console::UseFrame : Console::NoFrame);
	auto width = GameSetting::get().map.size.Value();
	auto height = GameSetting::get().map.size.Value();
	canvas.setClientSize(width, height);

	PlayGround playground(this->canvas);
	playground.play();

	if (GameData::get().retry_game)
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
	canvas.pushCursorOffset((width2 - width1) / 2, (height2 - height1) / 2);
	finally { canvas.popCursorOffset(); };

	DemoGround demoground(this->canvas);
	demoground.show();

	GameData::get().selection = PageSelect::BeginPage;
}

/***************************************
 class AboutPage
****************************************/
void AboutPage::run()
{
	int msg = MessageBoxW(Console::get().console_handle,
						  (~Token::about_text).c_str(),
						  (~Token::about_caption).c_str(),
						  MB_OK);
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
	print(Resource::GameTitle);
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
	canvas.setClientSize(DefaultSize);
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
				SoundPlayer::get().play(Sounds::Entrance);
				return;
			case K_2:
				GameData::get().selection = PageSelect::SettingPage;
				SoundPlayer::get().play(Sounds::Entrance);
				return;
			case K_3:
				GameData::get().selection = PageSelect::RankPage;
				SoundPlayer::get().play(Sounds::Entrance);
				return;
			case K_a:
			case K_A:
			case K_F1:
				GameData::get().selection = PageSelect::AboutPage;
				SoundPlayer::get().play(Sounds::About);
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
	canvas.setClientSize(DefaultSize);
	paintInterface();

	while (true)
	{
		paintCurOptions();
		switch (getwch())
		{
			case K_1:
				GameSetting::get().speed.setNextValue();
				SoundPlayer::get().play(Sounds::Switch);
				break;

			case K_2:
				GameSetting::get().map.set.setNextValue();
				SoundPlayer::get().play(Sounds::Switch);
				break;

			case '@':
				GameSetting::get().map.size.setNextValue();
				SoundPlayer::get().play(Sounds::Switch);
				break;

			case K_F2:
				GameData::get().selection = PageSelect::CustomMapPage;
				SoundPlayer::get().play(Sounds::Entrance);
				{
					auto page = Page::Create();
					page->run();
				}
				GameSaving::get().save();
				canvas.setClientSize(DefaultSize);
				paintInterface();
				break;

			case K_3:
				if (!GameSetting::get().old_console_host)
				{
					GameSetting::get().show_frame = !GameSetting::get().show_frame;
					SoundPlayer::get().play(Sounds::Switch);
				}
				break;

			case K_4:
				GameSetting::get().opening_pause = !GameSetting::get().opening_pause;
				SoundPlayer::get().play(Sounds::Switch);
				break;

			case K_5:
				GameSetting::get().theme.setNextValue();
				SoundPlayer::get().play(Sounds::Switch);
				break;

			case K_F5:
				GameData::get().selection = PageSelect::CustomThemePage;
				SoundPlayer::get().play(Sounds::Entrance);
				{
					auto page = Page::Create();
					page->run();
				}
				canvas.setClientSize(DefaultSize);
				paintInterface();
				break;

			case K_6:
				LocalizedStrings::setLang(
					GameSetting::get().lang.setNextValue().Value()
				);
				Console::get().setTitle(~Token::console_title);
				canvas.clear();
				paintInterface();
				SoundPlayer::get().play(Sounds::Switch);
				break;

			case K_7:
				GameSetting::get().mute = !GameSetting::get().mute;
				SoundPlayer::get().play(Sounds::Switch);
				break;

			case K_Enter:
				GameData::get().selection = PageSelect::MenuPage;
				GameSaving::get().save();
				SoundPlayer::get().play(Sounds::Confirm);
				return;

			case K_Esc:
				GameData::get().selection = PageSelect::MenuPage;
				// restore settings...
				GameSetting::get() = setting_backup;
				LocalizedStrings::setLang(setting_backup.lang.Value());
				if (custom_theme_backup)
					GameSetting::get().theme.SetCustomValue(*custom_theme_backup);
				else
					GameSetting::get().theme.ClearCustomValue();
				// ...excluding custom maps
				SoundPlayer::get().play(Sounds::Cancel);
				return;
		}
	}
}

void SettingPage::paintInterface()
{
	paintTitle(ShowVersion::No);

	auto [baseX, baseY] = canvas.getClientSize();
	baseX = baseX / 3 + 1;
	baseY = baseY / 2;
	canvas.setColor(Color::White);

	canvas.setCursor(baseX, baseY);
	print(~Token::setting_speed);
	canvas.setCursor(baseX - 1, baseY + 2);
	print(~Token::setting_map);
	canvas.setCursor(baseX - 10, baseY + 2);
	print(~Token::setting_customize_map);
	canvas.setCursor(baseX, baseY + 4);
	print(~Token::setting_show_frame);
	canvas.setCursor(baseX, baseY + 6);
	print(~Token::setting_opening_pause);
	canvas.setCursor(baseX, baseY + 8);
	print(~Token::setting_theme);
	canvas.setCursor(baseX - 9, baseY + 8);
	print(~Token::setting_customize_theme);
	canvas.setCursor(baseX, baseY + 10);
	print(~Token::setting_language);
	canvas.setCursor(baseX, baseY + 12);
	print(~Token::setting_mute);
	canvas.setCursor(baseX - 2, baseY + 14);
	print(~Token::setting_save);
	canvas.setCursor(baseX - 1, baseY + 16);
	print(~Token::setting_return);
}

void SettingPage::paintCurOptions()
{
	auto [baseX, baseY] = canvas.getClientSize();
	baseX = baseX / 2 + 4;
	baseY = baseY / 2;
	canvas.setColor(Color::White);

	canvas.setCursor(baseX, baseY);
	print(~GameSetting::get().speed.Name());

	canvas.setCursor(baseX, baseY + 2);
	print(GameSetting::get().map.set.Name());
	print(L" - ");
	print(GameSetting::get().map.size.Name());

	canvas.setCursor(baseX, baseY + 4);
	print(GameSetting::get().show_frame
		  ? ~Token::setting_yes
		  : ~Token::setting_no);

	canvas.setCursor(baseX, baseY + 6);
	print(GameSetting::get().opening_pause
		  ? ~Token::setting_yes
		  : ~Token::setting_no);

	canvas.setCursor(baseX, baseY + 8);
	print(~GameSetting::get().theme.Name());

	canvas.setCursor(baseX, baseY + 10);
	print(GameSetting::get().lang.Name());

	canvas.setCursor(baseX, baseY + 12);
	print(GameSetting::get().mute
		  ? ~Token::setting_yes
		  : ~Token::setting_no);
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
	canvas.setClientSize(DefaultSize);
	paintInterface();

	while (true)
	{
		paintCurOptions();
		switch (getwch())
		{
			case K_Q: case K_q:
				theme_temp[Element::Blank].facade.setNextValue();
				SoundPlayer::get().play(Sounds::Switch);
				break;
			case K_W: case K_w:
				theme_temp[Element::Food].facade.setNextValue();
				SoundPlayer::get().play(Sounds::Switch);
				break;
			case K_E: case K_e:
				theme_temp[Element::Snake].facade.setNextValue();
				SoundPlayer::get().play(Sounds::Switch);
				break;
			case K_R: case K_r:
				theme_temp[Element::Barrier].facade.setNextValue();
				SoundPlayer::get().play(Sounds::Switch);
				break;

			case K_A: case K_a:
				theme_temp[Element::Blank].color.setNextValue();
				SoundPlayer::get().play(Sounds::Switch);
				break;
			case K_S: case K_s:
				theme_temp[Element::Food].color.setNextValue();
				SoundPlayer::get().play(Sounds::Switch);
				break;
			case K_D: case K_d:
				theme_temp[Element::Snake].color.setNextValue();
				SoundPlayer::get().play(Sounds::Switch);
				break;
			case K_F: case K_f:
				theme_temp[Element::Barrier].color.setNextValue();
				SoundPlayer::get().play(Sounds::Switch);
				break;

			case K_Ctrl_Dd:
				GameSetting::get().theme.ClearCustomValue();
				GameData::get().selection = PageSelect::SettingPage;
				SoundPlayer::get().play(Sounds::Confirm);
				return;
			case K_Ctrl_Rr:
				generateRandomTheme();
				SoundPlayer::get().play(Sounds::Cancel);
				break;

			case K_Enter:
				GameSetting::get().theme = Theme::Custom;
				[[fallthrough]];
			case K_Esc:
				GameSetting::get().theme.SetCustomValue(theme_temp);
				GameData::get().selection = PageSelect::SettingPage;
				SoundPlayer::get().play(Sounds::Entrance);
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
	print(~Token::custom_theme_clear_custom);
	canvas.setCursor(18, 30);
	print(~Token::custom_theme_randomize);

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
	for (auto i : range<size_t>(Element::Mask_))
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
CustomMapPage::MapSelector::MapSelector(Canvas& canvas, Map& map)
	: canvas(canvas), map(map)
{
	if (MapSet::GetCount() < Map::MaxMapSetCount)
		MapSet::AddCustomItem({}, TempMapSetName);
}

CustomMapPage::MapSelector::~MapSelector() noexcept
{
	MapSet::RemoveCustomItem(TempMapSetName);
}

void CustomMapPage::MapSelector::selectPrev()
{
	if (map.set.Index() == 0)
		return;
	if (map.set.Index() - view_begin == 0 && map.set.Index() != 0)
		view_begin--;
	map.set.setPrevValue();
	refreshMapList();
}

void CustomMapPage::MapSelector::selectNext()
{
	if (map.set.Index() == MapSet::GetCount() - 1)
		return;
	if (map.set.Index() - view_begin == ViewSpan - 1 && map.set.Index() != MapSet::GetCount() - 1)
		view_begin++;
	map.set.setNextValue();
	refreshMapList();
}

DynArray<Element, 2> CustomMapPage::MapSelector::fetchSelected()
{
	DynArray<Element, 2> map_shape(map.size.Value(), map.size.Value());
	auto f = [&](const auto& m)
	{
		assert(map_shape.total_size() == m.size());
		std::copy(m.begin(), m.end(), map_shape.iter_all().begin());
	};
	map.visitValue(f);
	return map_shape;
}

void CustomMapPage::MapSelector::replaceSelected(const DynArray<Element, 2>& map_shape)
{
	assert(map_shape.total_size() == map.size.Value() * map.size.Value());
	auto f = [&](auto& m)
	{
		using type = std::remove_reference_t<decltype(m)>;
		m = type(map_shape.iter_all().begin(), map_shape.iter_all().end());
	};
	map.visitCustomValue(f);
	if (map.set.Name() == TempMapSetName)
	{
		MapSet::RenameCustomItem(map.set, L"Unnamed"_crypt);
		if (MapSet::GetCount() < Map::MaxMapSetCount)
			MapSet::AddCustomItem({}, TempMapSetName);
	}
	refreshMapList();
}

void CustomMapPage::MapSelector::deleteSelected()
{
	MapSet temp = map.set;
	temp.setPrevValue();
	if (map.set.Name() == TempMapSetName)
		return;
	MapSet::RemoveCustomItem(map.set);
	if (MapSet(MapSet::GetCount() - 1).Name() != TempMapSetName)
		MapSet::AddCustomItem({}, TempMapSetName);
	map.set = temp.setNextValue();
	refreshMapList();
}

void CustomMapPage::MapSelector::paint()
{
	canvas.pushCursorOffset(CanvasOffsetX, CanvasOffsetY);
	canvas.setColor(NormalColor);
	canvas.setCursor(2, 0);
	for (auto i : range(ViewSpan))
		print(L"  /------------\\ " + !!i);
	canvas.setCursor(2, 2);
	for (auto i : range(ViewSpan))
		print(L"  \\------------/ " + !!i);
	assert(map.set.Index() == 0);
	canvas.popCursorOffset();
	refreshMapList();
}

void CustomMapPage::MapSelector::renameCurrentMapSet()
{
	canvas.pushCursorOffset(CanvasOffsetX, CanvasOffsetY);
	canvas.setCursor(static_cast<short>(4 + 8 * (map.set.Index() - view_begin)), 1);
	canvas.setColor(Color::LightYellow);
	std::wstring name;
	for (wchar_t ch;;)
	{
		ch = getwchar();
		if (ch == L'\n')
			break;
		if (iswprint(ch) && ch != L' ')
			name += ch;
	}
	if (StrFullWidthLength(name) > NameMaxFullWidth)
		name.resize(StrIndexOfFullWidthLength(name, NameMaxFullWidth));
	if (!name.empty() && name != TempMapSetName)
		MapSet::RenameCustomItem(map.set, std::move(name));
	canvas.popCursorOffset();
	refreshMapList();
}

void CustomMapPage::MapSelector::refreshMapList()
{
	canvas.pushCursorOffset(CanvasOffsetX, CanvasOffsetY);
	finally { canvas.popCursorOffset(); };
	canvas.setColor(NormalColor);
	canvas.setCursor(2, 1);
	for (auto i : range(ViewSpan))
		print(L"  |            | " + !!i);

	MapSet set(view_begin);
	canvas.setCursor(2, 1);
	for (auto i : range(ViewSpan))
	{
		if (set == map.set)
			canvas.setColor(HighlightColor);
		else
			canvas.setColor(NormalColor);
		if (set.Name() == TempMapSetName)
		{
			print(L"  |   --++--   | " + !!i);
			break;
		}
		print(::format((L" " + !!i) + L" |{:02}{: <10.10}| "_crypt, view_begin + i + 1, set.Name()));
		set.setNextValue();
	}
}

void CustomMapPage::MapViewer::changeMap(DynArray<Element, 2> map)
{
	editing_map = std::move(map);
	paint();
}

void CustomMapPage::MapViewer::enterEditing()
{
	assert(!is_editing);
	is_editing = true;
	paintSelectedPos(HighlightColor);
}

const DynArray<Element, 2>& CustomMapPage::MapViewer::exitEditing()
{
	assert(is_editing);
	is_editing = false;
	paintSelectedPos(NormalColor);
	return editing_map;
}

void CustomMapPage::MapViewer::moveSelected(Direction direct)
{
	paintSelectedPos(NormalColor);
	switch (direct)
	{
		case Direction::Up:
			y == 0 ? y = editing_map.size() - 1 : y--;
			break;
		case Direction::Down:
			y == editing_map.size() - 1 ? y = 0 : y++;
			break;
		case Direction::Left:
			x == 0 ? x = editing_map.size() - 1 : x--;
			break;
		case Direction::Right:
			x == editing_map.size() - 1 ? x = 0 : x++;
			break;
	}
	paintSelectedPos(HighlightColor);
}

void CustomMapPage::MapViewer::switchSelected()
{
	switch (editing_map[y][x])
	{
		case Element::Blank:
			editing_map[y][x] = Element::Barrier; break;
		case Element::Barrier:
			editing_map[y][x] = Element::Blank; break;
	}
	paintSelectedPos(HighlightColor);
}

void CustomMapPage::MapViewer::setAllBlank()
{
	for (auto& node : editing_map.iter_all())
		node = Element::Blank;
	paint();
	paintSelectedPos(HighlightColor);
}

void CustomMapPage::MapViewer::paint() const
{
	canvas.pushCursorOffset(CanvasOffsetX, CanvasOffsetY);
	finally { canvas.popCursorOffset(); };
	canvas.setColor(NormalColor);

	std::wstring line;
	auto map_begin_pos = static_cast<unsigned short>((Size(Size::L).Value() - editing_map.size()) / 2);
	auto iter = editing_map.iter_all().begin();
	for (auto row : range<unsigned short>(Size(Size::L).Value()))
	{
		canvas.setCursor(0, row);
		line.clear();
		for (auto column : range<unsigned short>(Size(Size::L).Value()))
		{
			if (row >= map_begin_pos && column >= map_begin_pos &&
				row < map_begin_pos + editing_map.size() &&
				column < map_begin_pos + editing_map.size())
			{
				assert(iter < editing_map.iter_all().end());
				switch (*iter++)
				{
					case Element::Blank:
						line += L'□'; break;
					case Element::Barrier:
						line += L'■'; break;
				}
			}
			else
				line += L"  ";
		}
		print(line);
	}
}

void CustomMapPage::MapViewer::paintSelectedPos(Color color) const
{
	canvas.pushCursorOffset(CanvasOffsetX, CanvasOffsetY);
	finally { canvas.popCursorOffset(); };
	auto map_begin_pos = (Size(Size::L).Value() - editing_map.size()) / 2;
	auto pos_x = static_cast<short>(map_begin_pos + x);
	auto pos_y = static_cast<short>(map_begin_pos + y);
	canvas.setCursor(pos_x, pos_y);
	canvas.setColor(color);
	switch (editing_map[y][x])
	{
		case Element::Blank:
			print(L'□'); break;
		case Element::Barrier:
			print(L'■'); break;
	}
}

void CustomMapPage::run()
{
	canvas.setClientSize(DefaultSize);
	paintInterface();
	map_list.paint();
	map_viewer.changeMap(map_list.fetchSelected());
	map_viewer.paint();

	while (true)
	{
		paintCurOptions();
		switch (editor_state)
		{
			case EditorState::MapSelect:
				switch (getwch())
				{
					case K_F1:
						map_list.selectPrev();
						map_viewer.changeMap(map_list.fetchSelected());
						SoundPlayer::get().play(Sounds::Switch);
						break;
					case K_F2:
						map_list.selectNext();
						map_viewer.changeMap(map_list.fetchSelected());
						SoundPlayer::get().play(Sounds::Switch);
						break;
					case K_F3:
						if (MapSet::IsCustomItem(map.set))
						{
							canvas.pushCursorOffset(CanvasOffsetX, CanvasOffsetY);
							canvas.setCursor(2, 2);
							canvas.setColor(HighlightColor);
							print(~Token::custom_map_edit_map);
							canvas.popCursorOffset();
							SoundPlayer::get().play(Sounds::Entrance);

							map_viewer.enterEditing();
							editor_state = EditorState::MapEdit;
						}
						break;
					case K_F4:
						map.size.setNextValue();
						map_viewer.changeMap(map_list.fetchSelected());
						SoundPlayer::get().play(Sounds::Switch);
						break;
					case K_F5:
						if (MapSet::IsCustomItem(map.set) && map.set.Name() != MapSelector::TempMapSetName)
						{
							editor_state = EditorState::MapNaming;
							SoundPlayer::get().play(Sounds::Entrance);
						}
						break;
					case K_Delete:
						if (MapSet::IsCustomItem(map.set) && map.set.Name() != MapSelector::TempMapSetName)
						{
							canvas.pushCursorOffset(CanvasOffsetX, CanvasOffsetY);
							canvas.setCursor(11, 2);
							canvas.setColor(Color::LightRed);
							print(~Token::custom_map_delete_map_confirm);
							canvas.popCursorOffset();
							SoundPlayer::get().play(Sounds::Entrance);
							if (getwch() == K_Delete)
							{
								map_list.deleteSelected();
								map_viewer.changeMap(map_list.fetchSelected());
								SoundPlayer::get().play(Sounds::Confirm);
							}
							else
							{
								SoundPlayer::get().play(Sounds::Cancel);
							}
							canvas.pushCursorOffset(CanvasOffsetX, CanvasOffsetY);
							canvas.setCursor(11, 2);
							canvas.setColor(NormalColor);
							print(~Token::custom_map_delete_map);
							canvas.popCursorOffset();
						}
						break;
					case K_Enter: case K_Esc:
						GameData::get().selection = PageSelect::SettingPage;
						SoundPlayer::get().play(Sounds::Entrance);
						return;
				}
				break;
			case EditorState::MapEdit:
				switch (getwch())
				{
					case K_UP: case K_W: case K_w:
						map_viewer.moveSelected(MapViewer::Direction::Up);
						SoundPlayer::get().play(Sounds::Entrance);
						break;
					case K_DOWN: case K_S: case K_s:
						map_viewer.moveSelected(MapViewer::Direction::Down);
						SoundPlayer::get().play(Sounds::Entrance);
						break;
					case K_LEFT: case K_A: case K_a:
						map_viewer.moveSelected(MapViewer::Direction::Left);
						SoundPlayer::get().play(Sounds::Entrance);
						break;
					case K_RIGHT: case K_D: case K_d:
						map_viewer.moveSelected(MapViewer::Direction::Right);
						SoundPlayer::get().play(Sounds::Entrance);
						break;
					case K_Space:
						map_viewer.switchSelected();
						SoundPlayer::get().play(Sounds::Switch);
						break;
					case K_Ctrl_Bb:
						map_viewer.setAllBlank();
						SoundPlayer::get().play(Sounds::Switch);
						break;
					case K_Enter:
						if (auto& map = map_viewer.exitEditing();
							std::ranges::any_of(map.iter_all(), [](Element e) { return e == Element::Blank; }))
							map_list.replaceSelected(map);
						else // all barrier map is invalid
							map_viewer.changeMap(map_list.fetchSelected());
						paintInterface();
						editor_state = EditorState::MapSelect;
						SoundPlayer::get().play(Sounds::Confirm);
						break;
					case K_Esc:
						map_viewer.exitEditing();
						map_viewer.changeMap(map_list.fetchSelected());
						paintInterface();
						editor_state = EditorState::MapSelect;
						SoundPlayer::get().play(Sounds::Cancel);
						break;
				}
				break;
			case EditorState::MapNaming:
			{
				canvas.pushCursorOffset(CanvasOffsetX, CanvasOffsetY);
				canvas.setCursor(11, 4);
				canvas.setColor(HighlightColor);
				print(~Token::custom_map_rename);
				canvas.popCursorOffset();

				map_list.renameCurrentMapSet();
				
				canvas.pushCursorOffset(CanvasOffsetX, CanvasOffsetY);
				canvas.setCursor(11, 4);
				canvas.setColor(NormalColor);
				print(~Token::custom_map_rename);
				canvas.popCursorOffset();

				editor_state = EditorState::MapSelect;
				SoundPlayer::get().play(Sounds::Entrance);
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
	canvas.setCursor(0, 0);
	canvas.setColor(Color::LightAqua);
	print(custom_map_title);

	canvas.pushCursorOffset(CanvasOffsetX, CanvasOffsetY);
	finally { canvas.popCursorOffset(); };
	canvas.setColor(NormalColor);

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
	canvas.setCursor(11, 4);
	print(~Token::custom_map_rename);
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
	canvas.setColor(NormalColor);
	canvas.pushCursorOffset(CanvasOffsetX, CanvasOffsetY);
	finally { canvas.popCursorOffset(); };

	canvas.setCursor(11, 8);
	print(~Token::custom_map_curr_size);
	print(map.size.Name());
	canvas.setCursor(2, 8);
	print(~Token::custom_map_curr_pos);
	print(::format(L"({:>2},{:<2})"_crypt, map_viewer.getX() + 1, map_viewer.getY() + 1));
}

/***************************************
 class BeginPage
****************************************/
void BeginPage::run()
{
	canvas.setClientSize(DefaultSize);
	paintInterface();

	(void)getwch();
	*is_press = true;
	GameData::get().selection = PageSelect::MenuPage;
	SoundPlayer::get().play(Sounds::Intro);
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
					print(Resource::GameTitle);
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
					print(Resource::GameTitle);
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
	canvas.setClientSize(DefaultSize);
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
				GameData::get().selection = PageSelect::MenuPage;
				SoundPlayer::get().play(Sounds::Confirm);
				return;

			case K_Enter:
			case K_Esc:
				GameData::get().selection = PageSelect::MenuPage;
				SoundPlayer::get().play(Sounds::Entrance);
				return;
		}
	}
}

void RankPage::paintInterface()
{
	using namespace std::chrono_literals;
	paintTitle(ShowVersion::No);

	auto [baseX, baseY] = canvas.getClientSize();
	if (auto [rank, lock] = Rank::get().getRank(); rank[0].score == 0)
	{
		canvas.setCursorCentered(~Token::rank_no_data, baseY * 2 / 3);
		canvas.setColor(Color::LightYellow);
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
			buffer += ::format(L"{:<{}}"_crypt, std::move(name), Rank::NameMaxLength);
			buffer += ::format(L" {:>4.4}"_crypt, item.is_win ? ~Token::rank_win : std::to_wstring(item.score));
			buffer += L" | "_crypt;
			buffer += ~Token::rank_setting;
			speed = ~Speed::GetNameFrom(item.speed);
			buffer += ::format(L"{:<{}} "_crypt, std::move(speed), 6);
			buffer += ::format(L"{0:>{1}.{1}} - {2}"_crypt,
							   item.map_name, Map::NameMaxHalfWidth, Size::GetNameFrom(item.size));
			buffer = ::format(L"{:^{}}"_crypt, std::move(buffer), baseX * 2);

			canvas.setColor(item.is_win ? Color::LightGreen : Color::LightYellow);
			print(buffer);
		}

		std::this_thread::sleep_for(50ms);
		canvas.setColor(Color::White);
		canvas.setCursor(baseX / 9, baseY + 13);
		print(~Token::rank_clear_all_records);
	}
	std::this_thread::sleep_for(500ms);
}