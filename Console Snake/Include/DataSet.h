#pragma once
#ifndef SNAKE_DATASET_HEADER_
#define SNAKE_DATASET_HEADER_

#include "Canvas.h"
#include "Resource.h"
#include "Singleton.h"

enum struct PageSel
{
	BeginPage,
	MenuPage,
	GamePage,
	SettingPage,
	RankPage,
	AboutPage
};

struct GameDataMember
{
	PageSel seletion = PageSel::BeginPage;
	int score = 0;
	bool retry_game = false;
};
using GameData = Singleton<GameDataMember>;

struct GameSettingMember
{
	Speed speed;
	Lang lang;
	Size width;
	Size height;
	bool show_frame = false;
	// The old console host automatically Line Feed when printing the last character of line.
	// The new console host doesn't have the same behavior and work whether print(L'\n') or not.
	bool old_console_host = false;
};
using GameSetting = Singleton<GameSettingMember>;


enum struct GameElementTag
{
	blank = 1,
	food,
	snake,
	barrier
};

struct Appearance
{
	wchar_t facade;
	Color color;
};

template<GameElementTag Level>
struct GameElement
{
	static Appearance appearance;
};

inline Appearance GameElement<GameElementTag::blank>::appearance = { L'□', Color::Blue };
inline Appearance GameElement<GameElementTag::food>::appearance = { L'★', Color::Red };
inline Appearance GameElement<GameElementTag::snake>::appearance = { L'●', Color::Yellow };
inline Appearance GameElement<GameElementTag::barrier>::appearance = { L'■', Color::Green };

#endif // SNAKE_DATASET_HEADER_