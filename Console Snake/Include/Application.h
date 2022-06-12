#pragma once
#ifndef SNAKE_APPLICATION_HEADER_
#define SNAKE_APPLICATION_HEADER_

#include "GlobalData.h"

class Application
{
public:
	Application(int argc, char* argv[]);
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

public:
	int run();

private:
	GameData game_data;
	GameSetting game_setting;
};

#endif // SNAKE_APPLICATION_HEADER_