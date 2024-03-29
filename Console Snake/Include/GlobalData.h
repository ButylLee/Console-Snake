﻿#pragma once
#ifndef SNAKE_GLOBALDATA_HEADER_
#define SNAKE_GLOBALDATA_HEADER_

#include "Resource.h"
#include "GlobalResourceWrapper.h"
#include "PageInterface.h"

struct GameDataMember
{
	PageSelect selection = PageSelect::BeginPage;
	int score = 0;
	bool exit_game = false;
	bool retry_game = false;
	bool colorful_title = false;
};
using GameData = GlobalResourceWrapper<GameDataMember>;

struct GameSettingMember
{
	Speed speed;
	Lang lang;
	Map map;
	Theme theme;
	bool show_frame = false;
	bool opening_pause = true;
	bool mute = false;
	// The old console host automatically Line Feed when printing the last character of line.
	// The new console host doesn't have the same behavior and work whether print(L'\n') or not.
	bool old_console_host = false;
};
using GameSetting = GlobalResourceWrapper<GameSettingMember>;

#endif // SNAKE_GLOBALDATA_HEADER_