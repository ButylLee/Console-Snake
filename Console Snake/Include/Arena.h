#pragma once
#ifndef SNAKE_ARENA_HEADER_
#define SNAKE_ARENA_HEADER_

#include "Canvas.h"
#include "DynArray.h"
#include "Resource.h"
#include <atomic>
#include <cstdint>

struct Direction
{
	enum Tags
	{
		None = 0,
		Up = 1,
		Left = 2,
		Right = 3,
		Down = 4,
		Conflict = 5,
	};

	constexpr Direction() noexcept :value(None) {}
	constexpr Direction(Tags tag) noexcept :value(tag) {}

	friend constexpr bool operator==(Direction, Direction) = default;
	friend constexpr Tags operator+(Direction direction) noexcept // for 2 times type conversion
	{
		return direction.value;
	}
	friend constexpr Direction operator+(Tags tag) noexcept // for 2 times type conversion
	{
		return tag;
	}

	constexpr bool isConflictWith(Direction other) noexcept
	{
		return this->value + other.value == Conflict;
	}

private:
	Tags value;
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
	void paintElement(Element which) noexcept;
	void paintElement(Element which, short x, short y) noexcept;
	bool isOver() const noexcept;
	bool isWin() const noexcept;
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
	std::atomic<Direction> input_key = +Direction::None;

private:
	Canvas& canvas;
	Direction snake_direct;
	bool game_over = false;

	// The invariant of this class is that ALL map nodes except barrier
	// and snake_body nodes should have one-to-one correspondence.
	DynArray<MapNode, 2> map;
	DynArray<SnakeNode> snake_body;
	int16_t snake_head;
	int16_t snake_tail;
};

#endif // SNAKE_ARENA_HEADER_