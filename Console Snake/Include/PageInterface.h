#pragma once
#ifndef SNAKE_PAGEINTERFACE_HEADER_
#define SNAKE_PAGEINTERFACE_HEADER_

#include "Interface.h"
#include <cassert>
#include <memory>
#include <unordered_map>

#pragma region FactoryMacros

#define FUNC_GET_MAP _GetMap

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

#define FACTORY_MAP_DEFINE(class_name) FACTORY_MAP_DEFINE_(class_name, KEY_SCOPE)
#define FACTORY_MAP_REGISTER(class_name) FACTORY_MAP_REGISTER_(class_name, KEY_SCOPE)

#pragma endregion

enum struct PageSelect
{
	GamePage,
	DemoPage,
	AboutPage,
	MenuPage,
	SettingPage,
	CustomThemePage,
	CustomMapPage,
	BeginPage,
	RankPage
};
#define KEY_SCOPE PageSelect

class Page :public Interface
{
	FACTORY_MAP_DEFINE(Page);
public:
	static std::unique_ptr<Page> Create();

public:
	virtual void run() = 0;
};

#undef FACTORY_MAP_DEFINE_
#undef FACTORY_MAP_DEFINE

#endif // SNAKE_PAGEINTERFACE_HEADER_