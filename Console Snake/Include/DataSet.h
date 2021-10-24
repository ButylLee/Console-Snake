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

struct GameCustomDataMember
{

};
using GameCustomData = Singleton<GameCustomDataMember>;

#endif // SNAKE_DATASET_HEADER_