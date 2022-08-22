#pragma once
#ifndef SNAKE_PLAYGROUND_HEADER_
#define SNAKE_PLAYGROUND_HEADER_

#include "Canvas.h"
#include "Arena.h"
#include <atomic>
#include <chrono>

class PlayGround
{
	static constexpr std::chrono::milliseconds pause_flicker_interval{ 500 };
	enum struct GameStatus
	{
		Running, Pausing, Ending
	};

public:
	PlayGround(Canvas& canvas);
	PlayGround(const PlayGround&) = delete;
	PlayGround& operator=(const PlayGround&) = delete;

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