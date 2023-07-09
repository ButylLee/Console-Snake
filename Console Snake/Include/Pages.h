#pragma once
#ifndef SNAKE_PAGES_HEADER_
#define SNAKE_PAGES_HEADER_

#include "PageInterface.h"
#include "Canvas.h"
#include "GlobalData.h"
#include "Resource.h"
#include "DynArray.h"
#include <memory>
#include <optional>

class GamePage :public Page
{
	FACTORY_MAP_REGISTER(GamePage);
public:
	void run() override;

private:
	Canvas canvas;
};

class DemoPage :public Page
{
	FACTORY_MAP_REGISTER(DemoPage);
public:
	void run() override;

private:
	Canvas canvas;
};

class AboutPage :public Page
{
	FACTORY_MAP_REGISTER(AboutPage);
public:
	void run() override;
};

class NormalPage :public Page
{
public:
	void run() override = 0;

protected:
	static constexpr ClientSize default_size = { 45,35 };
	enum struct ShowVersion { No, Yes };

protected:
	void paintTitle(ShowVersion);
	Canvas canvas;
};

class MenuPage :public NormalPage
{
	FACTORY_MAP_REGISTER(MenuPage);
public:
	void run() override;

private:
	void paintInterface();
};

class SettingPage :public NormalPage
{
	FACTORY_MAP_REGISTER(SettingPage);
public:
	void run() override;

private:
	void paintInterface();
	void paintCurOptions();
	GameSetting setting_backup = GameSetting::get();
	std::optional<ElementSet> custom_theme_backup = GameSetting::get().theme.GetCustomValue();
};

class CustomThemePage :public NormalPage
{
	FACTORY_MAP_REGISTER(CustomThemePage);
public:
	CustomThemePage() noexcept;
	void run() override;

private:
	void paintInterface();
	void paintCurOptions();
	void generateRandomTheme();
	ElementSet theme_temp;
};

class CustomMapPage :public NormalPage
{
	FACTORY_MAP_REGISTER(CustomMapPage);
private:
	class MapSelector
	{
		static constexpr size_t view_span = 5;
		static constexpr size_t max_mapset_count = 16;
		static constexpr const wchar_t* temp_mapset_name = L"$";
		static constexpr short canvas_offset_x = 0, canvas_offset_y = 6;
	public:
		MapSelector(Canvas&, Map&);
		~MapSelector() noexcept;
		void selectPrev();
		void selectNext();
		DynArray<Element, 2> fetchSelected();
		void replaceSelected(const DynArray<Element, 2>&);
		void deleteSelected();
		void paint();
	private:
		void refreshMapList();
	private:
		Canvas& canvas;
		Map& map;
		size_t view_begin = 0;
	};
	class MapViewer
	{
		static constexpr short canvas_offset_x = 1, canvas_offset_y = 10;
	public:
		enum struct Direction { Up, Down, Left, Right };
	public:
		MapViewer(Canvas& canvas) :canvas(canvas) {}
		void changeMap(DynArray<Element, 2>);
		void enterEditing();
		const DynArray<Element, 2>& exitEditing();
		void moveSelected(Direction);
		void switchSelected();
		void setAllBlank();
		void paint() const;
		size_t getX() const noexcept { return x; }
		size_t getY() const noexcept { return y; }
	private:
		Canvas& canvas;
		DynArray<Element, 2> editing_map;
		size_t x = 0, y = 0;
		bool is_editing = false;
	};
	enum struct EditorLevel { MapSelect, MapEdit, MapNaming };

public:
	void run() override;

private:
	void paintInterface();
	void paintCurOptions();

private:
	EditorLevel editor_level = EditorLevel::MapSelect;
	Map map;
	MapSelector map_list{ canvas, map };
	MapViewer map_viewer{ canvas };
};

class BeginPage :public NormalPage
{
	FACTORY_MAP_REGISTER(BeginPage);
public:
	void run() override;

private:
	void paintInterface();
	std::shared_ptr<bool> is_press = std::make_shared<bool>(false);
};

class RankPage :public NormalPage
{
	FACTORY_MAP_REGISTER(RankPage);
public:
	void run() override;

private:
	void paintInterface();
	bool is_no_data = false;
};

#endif // SNAKE_PAGES_HEADER_