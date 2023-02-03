#pragma once
#ifndef SNAKE_APPLICATION_HEADER_
#define SNAKE_APPLICATION_HEADER_

#include "Interface.h"
#include "GlobalData.h"

void EnsureOnlyOneInstance() noexcept;

class Application :NotCopyable
{
public:
	Application(int argc, char* argv[]);

public:
	int run();

private:
	GameData game_data;
	GameSetting game_setting;
};

#endif // SNAKE_APPLICATION_HEADER_