#pragma once
#ifndef SNAKE_PAGES_HEADER_
#define SNAKE_PAGES_HEADER_

#include "Canvas.h"
#include "DataSet.h"
#include <memory>

class Page
{
public:
	virtual void run() = 0;
	virtual ~Page() = default;
};

class GamePage :public Page
{
public:
	void run() override;

private:
	Canvas canvas;
};

class AboutPage :public Page
{
public:
	void run() override;
};

class NormalPage :public Page
{
public:
	void run() override = 0;

protected:
	static constexpr ClientSize default_size = { 45,35 };
	enum struct ShowVersion
	{
		Yes, No
	};
protected:
	void paintTitle(ShowVersion);
	Canvas canvas;
};

class MenuPage :public NormalPage
{
public:
	void run() override;

private:
	void paintInterface();
};

class SettingPage :public NormalPage
{
public:
	void run() override;

private:
	void paintInterface();
	void paintCurOptions();
	GameSettingMember setting_backup = GameSetting::get();
	std::optional<ElementSet> custom_theme_backup = GameSetting::get().theme.getCustomValue();
};

class CustomThemePage :public NormalPage
{
public:
	CustomThemePage() noexcept;
	void run() override;

private:
	void paintInterface();
	void paintCurOptions();
	ElementSet theme_temp;
};

class BeginPage :public NormalPage
{
public:
	void run() override;

private:
	void paintInterface();
	static bool is_press;
};

class RankPage :public NormalPage
{
public:
	void run() override;

private:
	void paintInterface();
};

std::unique_ptr<Page> CreatePage();

#endif // SNAKE_PAGES_HEADER_