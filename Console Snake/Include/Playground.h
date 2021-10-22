#pragma once
#ifndef SNAKE_PLAYGROUND_HEADER_
#define SNAKE_PLAYGROUND_HEADER_

#include "Canvas.h"
#include "DataSet.h"
#include "DynArray.h"
#include <atomic>
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

struct MapNode
{
	int16_t snake_index;
	GameElementTag type;
};

struct SnakeNode
{
	uint8_t x;
	uint8_t y;
};

class Playground
{
	static constexpr int snake_begin_length = 3;
public:
	Playground(Canvas& canvas, std::atomic<Direction>& input_key,
			   std::atomic<Direction>& snake_direct, std::atomic<GameStatus>& game_status)
		: canvas(canvas), input_key(input_key)
		, snake_direct(snake_direct), game_status(game_status)
		, map(GameSetting::get().height, GameSetting::get().width)
		, snake_body(getSnakeBodySize())
	{
		setupInvariantAndPaint();
		createSnake();
		createFood();
		GameData::get().score = 0;
		game_status = GameStatus::Running;
	}

public:
	void play();

private:
	template<GameElementTag Which>
	constexpr void paintElement() noexcept;
	size_t getSnakeBodySize() noexcept;
	void setupInvariantAndPaint() noexcept;
	void createSnake();
	void createFood();
	auto& getRandomEngine();
	template<typename T>
	T getRandom(T min, T max);

	void updateFrame();
	void rebindData(int16_t snake_index, int8_t map_x, int8_t map_y) noexcept;
	void forwardIndex(int16_t& index) noexcept;
	void endGame();

private:
	Canvas& canvas;
	std::atomic<Direction>& input_key;
	std::atomic<Direction>& snake_direct;
	std::atomic<GameStatus>& game_status;

	// The invariant of this class is that ALL map nodes except barrier
	// and snake_body nodes should have one-to-one correspondence.
	DynArray<MapNode, 2> map;
	DynArray<SnakeNode, 1> snake_body;
	int16_t snake_head;
	int16_t snake_tail;

	bool game_over = false;
};

#endif // SNAKE_PLAYGROUND_HEADER_