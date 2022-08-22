#pragma once
#ifndef SNAKE_PAGES_HEADER_
#define SNAKE_PAGES_HEADER_

#include "Canvas.h"
#include "GlobalData.h"
#include <memory>
#include <unordered_map>
#include <cassert>

#pragma region FactoryMacros

#define FUNC_GET_MAP _getMap

#define FACTORY_MAP_DEFINE_(class_name, key_scope) \
protected: \
	static auto& FUNC_GET_MAP() \
	{ \
		static std::unordered_map<key_scope, std::unique_ptr<class_name>(*)()> map; \
		return map; \
	} \
	template<typename T> \
	static std::unique_ptr<class_name> _FactoryNew() \
	{ \
		return std::make_unique<T>(); \
	}

#define FACTORY_MAP_REGISTER_(class_name, key_scope) \
private: \
	inline static bool _is_registered = [] \
		{ \
			auto& map = class_name::FUNC_GET_MAP(); \
			/* avoid register more than once */ \
			assert(map.find(key_scope::class_name) == map.end()); \
			map[key_scope::class_name] = class_name::_FactoryNew<class_name>; \
			return true; \
		}()

#define KEY_SCOPE PageSelect
#define FACTORY_MAP_DEFINE(class_name) FACTORY_MAP_DEFINE_(class_name, KEY_SCOPE)
#define FACTORY_MAP_REGISTER(class_name) FACTORY_MAP_REGISTER_(class_name, KEY_SCOPE)

#pragma endregion

class Page
{
	FACTORY_MAP_DEFINE(Page);
public:
	static std::unique_ptr<Page> Create();

public:
	virtual void run() = 0;
	virtual ~Page() = default;
};

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
	std::optional<ElementSet> custom_theme_backup = GameSetting::get().theme.getCustomValue();
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
	ElementSet theme_temp;
};

class BeginPage :public NormalPage
{
	FACTORY_MAP_REGISTER(BeginPage);
public:
	void run() override;

private:
	void paintInterface();
	bool is_press = false; // benign data race
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

#undef DEFINE_FACTORY_MAP_
#undef DEFINE_FACTORY_MAP
#undef FACTORY_MAP_REGISTER_
#undef FACTORY_MAP_REGISTER
#undef KEY_SCOPE

#endif // SNAKE_PAGES_HEADER_