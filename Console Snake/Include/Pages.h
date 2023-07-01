#pragma once
#ifndef SNAKE_PAGES_HEADER_
#define SNAKE_PAGES_HEADER_

#include "PageInterface.h"
#include "Canvas.h"
#include "GlobalData.h"
#include <memory>

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

	};
	class MapSelectorList
	{
	public:
		MapSelectorList(Map& map) :map(map) {}
		void paint();
		void selectPrev();
		void selectNext();
		void deleteSelected();
	private:
		Map& map;
	};
	class MapViewer
	{
	public:
		enum struct Direction { Up, Down, Left, Right };
	public:
		MapViewer(Map& map) :map(map) {}
		void paint() const;
		void moveSelected(Direction);
		void switchSelected();
		void clearMap();
	private:
		Map& map;
		short x = 0, y = 0;
	};
	enum struct EditorLevel { MapSelect, MapEdit };

public:
	void run() override;

private:
	void paintInterface();
	void paintCurOptions();

private:
	EditorLevel editor_level = EditorLevel::MapSelect;
	Map map;
	MapSelectorList map_list{ map };
	MapViewer map_viewer{ map };
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