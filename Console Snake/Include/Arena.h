#pragma once
#ifndef SNAKE_ARENA_HEADER_
#define SNAKE_ARENA_HEADER_

#include "Canvas.h"
#include "DynArray.h"
#include "Resource.h"
#include <atomic>
#include <optional>
#include <cstdint>
#include <cstddef>

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
	friend constexpr Tags operator+(Direction direction) noexcept // for twice type conversion
	{
		return direction.value;
	}
	friend constexpr Direction operator+(Tags tag) noexcept // for twice type conversion
	{
		return tag;
	}
	friend constexpr Direction operator-(Direction direction) noexcept
	{
		return Tags(Direction::Conflict - direction.value);
	}

	constexpr bool isConflictWith(Direction other) const noexcept
	{
		return this->value + other.value == Conflict;
	}

private:
	Tags value;
};

struct MapNode
{
	Element type;
	int16_t snake_index = -1;
};

struct PosNode
{
	uint8_t x;
	uint8_t y;
};

struct PosNodeGroup
{
	size_t count;
	PosNode head_pos;
	PosNode tail_pos;
};

class Venue
{
public:
	Venue(DynArray<MapNode, 2> map, void (Venue::* create_snake_func)());

public:
	PosNode getNextPosition() const noexcept;
	Element getPositionType(uint8_t x, uint8_t y) const noexcept;
	const DynArray<MapNode, 2>& getCurrentMap() const noexcept;

protected:
	void addSnakeBody(Direction head_direct, uint8_t head_x, uint8_t head_y) noexcept;
	void addSnakeBody(Direction tail_direct) noexcept;
	std::optional<PosNode> generateFood();

	void orderDirection(Direction) noexcept;
	PosNodeGroup updateFrame() noexcept;

	bool isWin(size_t score, size_t snake_init_length) const noexcept;

private:
	void setupInvariant() noexcept;
	void rebindData(int16_t snake_index, int8_t map_x, int8_t map_y) noexcept;
	void forwardIndex(int16_t& index) const noexcept;
	void backwardIndex(int16_t& index) const noexcept;
	void nextPosition(uint8_t& x, uint8_t& y) const noexcept;

private:
	// The invariant of this class is that ALL map nodes except barrier
	// and snake_body nodes should have one-to-one correspondence.
	DynArray<MapNode, 2> map; // map[y][x]
	DynArray<PosNode> snake_body;
	int16_t snake_head_index = -1;
	int16_t snake_tail_index = -1;

	Direction snake_direct = Direction::None;
};

class Arena :public Venue
{
	static constexpr int snake_init_length = 3;
public:
	Arena(Canvas& canvas);

public:
	void updateFrame();
	void paintElement(Element);
	void paintElement(Element, uint8_t x, uint8_t y);
	bool isOver() const noexcept;
	bool isWin() const noexcept;

private:
	void paintVenue();
	void createSnake();
	void generateFood();

public:
	std::atomic<Direction> input_key = +Direction::None;

private:
	Canvas& canvas;
	bool game_over = false;
};

#endif // SNAKE_ARENA_HEADER_