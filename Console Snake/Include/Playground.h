#pragma once
#ifndef SNAKE_PLAYGROUND_HEADER_
#define SNAKE_PLAYGROUND_HEADER_

#include "Interface.h"
#include "Canvas.h"
#include "Arena.h"
#include <atomic>
#include <chrono>

class PlayGround :NotCopyable
{
	static constexpr std::chrono::milliseconds PauseFlickerInterval{ 500 };
	enum struct GameStatus
	{
		Running, Pausing, Ending
	};

public:
	PlayGround(Canvas& canvas);

public:
	void play();

private:
	void ending();

private:
	Canvas& canvas;
	Arena arena;
	std::atomic<GameStatus> game_status;
};

#endif // SNAKE_PLAYGROUND_HEADER_