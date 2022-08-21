#pragma once
#ifndef SNAKE_ARENA_HEADER_
#define SNAKE_ARENA_HEADER_

#include "Canvas.h"
#include "DynArray.h"
#include "Resource.h"
#include <atomic>
#include <cstdint>

enum struct Direction
{
	None = 0,
	Up = 1,
	Left = 2,
	Right = 3,
	Down = 4,
	Conflict = 5,
};

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

class Arena
{
	static constexpr int snake_begin_length = 3;
public:
	Arena(Canvas& canvas);

public:
	void updateFrame();
	bool is_over() noexcept;
	bool is_win() noexcept;
	void paintElement(Element which) noexcept;
	SnakeNode getNextPosition() const noexcept;
	Element getPositionType(uint8_t x, uint8_t y) const noexcept;
	const DynArray<MapNode, 2>& getCurrentMap() const noexcept;

private:
	void setupInvariantAndPaint() noexcept;
	void createSnake();
	void createFood();
	void rebindData(int16_t snake_index, int8_t map_x, int8_t map_y) noexcept;
	void forwardIndex(int16_t& index) const noexcept;
	void nextPosition(uint8_t& x, uint8_t& y) const noexcept;

public:
	std::atomic<Direction> input_key = Direction::None;

private:
	Canvas& canvas;
	std::atomic<Direction> snake_direct;
	bool game_over = false;

	// The invariant of this class is that ALL map nodes except barrier
	// and snake_body nodes should have one-to-one correspondence.
	DynArray<MapNode, 2> map;
	DynArray<SnakeNode> snake_body;
	int16_t snake_head;
	int16_t snake_tail;
};

#endif // SNAKE_ARENA_HEADER_