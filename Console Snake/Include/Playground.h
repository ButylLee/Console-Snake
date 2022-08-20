#pragma once
#ifndef SNAKE_PLAYGROUND_HEADER_
#define SNAKE_PLAYGROUND_HEADER_

#include "Canvas.h"
#include "GlobalData.h"
#include "DynArray.h"
#include "Resource.h"
#include <atomic>
#include "AtomicOperation.h"
#include <chrono>
#include <cstdint>

enum struct Direction
{
	None,
	Up, Down, Left, Right
};

enum struct GameStatus
{
	Running, Pausing, Ending
};

// --------------- class Playground ---------------
class Playground
{
	static constexpr int snake_begin_length = 3;
	static constexpr std::chrono::milliseconds pause_flicker_interval{ 500 };
	struct MapNode
	{
		int16_t snake_index;
		Element type;
	};
	struct SnakeNode
	{
		uint8_t x;
		uint8_t y;
	};

public:
	Playground(Canvas& canvas)
		: canvas(canvas), snake_body(getSnakeBodySize())
		, map(GameSetting::get().height, GameSetting::get().width)
	{
		setupInvariantAndPaint();
		createSnake();
		createFood();
		GameData::get().score = 0;
		storeAtomic(game_status, GameStatus::Running);
	}
	Playground(const Playground&) = delete;
	Playground& operator=(const Playground&) = delete;

public:
	void play();

private:
	void paintElement(Element which) noexcept;
	size_t getSnakeBodySize() noexcept;
	void setupInvariantAndPaint() noexcept;
	void createSnake();
	void createFood();

	void updateFrame();
	void rebindData(int16_t snake_index, int8_t map_x, int8_t map_y) noexcept;
	void forwardIndex(int16_t& index) noexcept;
	void nextPosition(uint8_t& x, uint8_t& y) noexcept;
	void endGame();

private:
	Canvas& canvas;
	std::atomic<Direction> input_key = Direction::None;
	std::atomic<Direction> snake_direct;
	std::atomic<GameStatus> game_status;
	bool game_over = false;

	// The invariant of this class is that ALL map nodes except barrier
	// and snake_body nodes should have one-to-one correspondence.
	DynArray<MapNode, 2> map;
	DynArray<SnakeNode> snake_body;
	int16_t snake_head;
	int16_t snake_tail;
};

#endif // SNAKE_PLAYGROUND_HEADER_