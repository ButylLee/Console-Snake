#include "Arena.h"
#include "GlobalData.h"
#include "WideIO.h"
#include "Random.h"

#include <utility>
#include <type_traits>
#include <cassert>
#include <algorithm>

namespace
{
	DynArray<MapNode, 2> GetMap()
	{
		DynArray<MapNode, 2> map(GameSetting::get().map.size.Value(),
								 GameSetting::get().map.size.Value());
		auto f = [&](const auto& m)
		{
			std::transform(m.begin(), m.end(), map.iter_all().begin(),
						   [](Element type)
						   {
							   MapNode node;
							   node.type = type;
							   return node;
						   });
		};
		GameSetting::get().map.applyValue(f);
		return map;
	}

	size_t GetMapBlankCount(const DynArray<MapNode, 2>& map)
	{
		size_t count = 0;
		for (auto& node : map.iter_all())
		{
			if (node.type == Element::Blank)
				count++;
		}
		return count;
	}
} // namespace

Venue::Venue(DynArray<MapNode, 2> map_, void (Venue::* create_snake_func)())
	: map(std::move(map_)), snake_body(GetMapBlankCount(map))
{
	setupInvariant();
	(this->*create_snake_func)();
	generateFood();
}

PosNode Venue::getNextPosition() const noexcept
{
	auto [x, y] = snake_body[snake_head_index];
	nextPosition(x, y);
	return { x, y };
}

Element Venue::getPositionType(uint8_t x, uint8_t y) const noexcept
{
	return map[y][x].type;
}

const DynArray<MapNode, 2>& Venue::getCurrentMap() const noexcept
{
	return map;
}

void Venue::addSnakeBody(Direction head_direct, uint8_t head_x, uint8_t head_y) noexcept
{
	assert(snake_head_index == -1 && snake_tail_index == -1 &&
		   snake_direct == Direction::None);
	snake_direct = head_direct;
	map[head_y][head_x].type = Element::Snake;
	snake_tail_index = snake_head_index = map[head_y][head_x].snake_index;
	rebindData(snake_head_index, head_x, head_y);
}

void Venue::addSnakeBody(Direction tail_direct) noexcept
{
	assert(snake_head_index != -1 && snake_tail_index != -1);
	if (tail_direct == snake_direct)
		return;
	auto [tail_x, tail_y] = snake_body[snake_tail_index];
	backwardIndex(snake_tail_index);
	switch (+tail_direct)
	{
		case Direction::Up:
			tail_y--; break;
		case Direction::Down:
			tail_y++; break;
		case Direction::Left:
			tail_x--; break;
		case Direction::Right:
			tail_x++; break;
	}
	map[tail_y][tail_x].type = Element::Snake;
	rebindData(snake_tail_index, tail_x, tail_y);
}

std::optional<PosNode> Venue::generateFood()
{
	size_t range;
	// get usable range for food generating
	if (snake_head_index > snake_tail_index)
	{
		range = snake_head_index - snake_tail_index - 1;
	}
	else
	{
		range = snake_body.total_size() - (snake_tail_index - snake_head_index + 1);
	}
	if (range == 0)
		return {};

	// pick random position of food
	size_t random_index = GetRandom(1, range) + snake_tail_index;
	if (random_index >= snake_body.total_size())
		random_index -= snake_body.total_size();

	// generate food on the map
	auto [x, y] = snake_body[random_index];
	map[y][x].type = Element::Food;
	return PosNode{ x, y };
}

void Venue::orderDirection(Direction input) noexcept
{
	assert(snake_head_index != -1 && snake_tail_index != -1);
	if (input != Direction::None && !input.isConflictWith(snake_direct))
		snake_direct = input;
}

PosNodeGroup Venue::updateFrame() noexcept
{
	auto [head_x, head_y] = snake_body[snake_head_index];
	nextPosition(head_x, head_y);

	auto previous_type = map[head_y][head_x].type;
	if (previous_type == Element::Barrier || previous_type == Element::Snake)
		return { 0 };

	map[head_y][head_x].type = Element::Snake;
	forwardIndex(snake_head_index);
	rebindData(snake_head_index, head_x, head_y);

	if (previous_type == Element::Food)
		return { 1, { head_x, head_y } };

	auto [tail_x, tail_y] = snake_body[snake_tail_index];
	map[tail_y][tail_x].type = Element::Blank;
	forwardIndex(snake_tail_index);
	// no need to rebind
	return { 2, { head_x, head_y }, { tail_x, tail_y } };
}

bool Venue::isWin(size_t score, size_t snake_init_length) const noexcept
{
	return score + snake_init_length == snake_body.size();
}

void Venue::setupInvariant() noexcept
{
	int16_t index = 0;
	for (uint8_t row = 0; row < map.size(0); row++)
	{
		for (uint8_t column = 0; column < map.size(1); column++)
		{
			if (map[row][column].type == Element::Blank)
			{
				map[row][column].snake_index = index;
				snake_body[index].x = column;
				snake_body[index].y = row;
				index++;
			}
		}
	}
}

void Venue::rebindData(int16_t index, int8_t x, int8_t y) noexcept
{
	// maintain class invariant
	int16_t temp_index = map[y][x].snake_index;
	uint8_t temp_x = snake_body[index].x;
	uint8_t temp_y = snake_body[index].y;
	std::swap(map[y][x].snake_index, map[temp_y][temp_x].snake_index);
	std::swap(snake_body[index], snake_body[temp_index]);
}

void Venue::forwardIndex(int16_t& index) const noexcept
{
	index = index == 0
		? static_cast<int16_t>(snake_body.total_size() - 1)
		: index - 1;
}

void Venue::backwardIndex(int16_t& index) const noexcept
{
	index = index == static_cast<int16_t>(snake_body.total_size() - 1)
		? 0
		: index + 1;
}

void Venue::nextPosition(uint8_t& x, uint8_t& y) const noexcept
{
	uint8_t height = static_cast<uint8_t>(map.size(0));
	uint8_t width = static_cast<uint8_t>(map.size(1));
	switch (+snake_direct)
	{
		case Direction::Up:
			y == 0 ? y = height - 1 : y--;
			break;
		case Direction::Down:
			y == height - 1 ? y = 0 : y++;
			break;
		case Direction::Left:
			x == 0 ? x = width - 1 : x--;
			break;
		case Direction::Right:
			x == width - 1 ? x = 0 : x++;
			break;
	}
}

Arena::Arena(Canvas& canvas)
	: Venue(GetMap(), static_cast<void(Venue::*)()>(&Arena::createSnake))
	, canvas(canvas)
{
	paintVenue();
}

void Arena::updateFrame()
{
	orderDirection(input_key.exchange(Direction::None));
	PosNodeGroup nodes_updated = Venue::updateFrame();
	assert(nodes_updated.count <= 2);
	switch (nodes_updated.count)
	{
		case 0: // dead
			game_over = true;
			break;
		case 1: // food
			paintElement(Element::Snake, nodes_updated.head_pos.x, nodes_updated.head_pos.y);
			generateFood();
			GameData::get().score++;
			break;
		case 2: // nothing
			paintElement(Element::Snake, nodes_updated.head_pos.x, nodes_updated.head_pos.y);
			paintElement(Element::Blank, nodes_updated.tail_pos.x, nodes_updated.tail_pos.y);
			break;
	}
}

void Arena::paintElement(Element which, uint8_t x, uint8_t y)
{
	canvas.setCursor(x, y);
	paintElement(which);
}

bool Arena::isOver() const noexcept
{
	return game_over;
}

bool Arena::isWin() const noexcept
{
	return Venue::isWin(GameData::get().score, snake_init_length);
}

void Arena::paintElement(Element which)
{
	canvas.setColor(GameSetting::get().theme.Value()[which].color);
	print(GameSetting::get().theme.Value()[which].facade.Value());
}

void Arena::paintVenue()
{
	canvas.setCursor(0, 0);
	auto& map = getCurrentMap();
	for (size_t index = 0; auto& node : map.iter_all())
	{
		if (not(GameSetting::get().old_console_host && index++ == map.total_size() - 1))
			paintElement(node.type);
	}
}

void Arena::createSnake()
{
	// random snake initial postion
	auto x_range = GameSetting::get().map.size.Value() - 2 * (1 + snake_init_length);
	auto y_range = GameSetting::get().map.size.Value() - 2 * (1 + snake_init_length);
	auto begin_head_x = GetRandom(0, x_range);
	auto begin_head_y = GetRandom(0, y_range);
	Direction direction;

	// select initial direction
	if (bool select_x_axis = GetRandom(0, 1))
	{
		if (begin_head_x < x_range / 2)
			direction = Direction::Right;
		else
			direction = Direction::Left;
	}
	else
	{
		if (begin_head_y < y_range / 2)
			direction = Direction::Down;
		else
			direction = Direction::Up;
	}
	begin_head_x += 1 + snake_init_length;
	begin_head_y += 1 + snake_init_length;

	// place initial snake body
	addSnakeBody(direction, begin_head_x, begin_head_y);
	for (size_t i = 0; i < snake_init_length - 1; i++)
		addSnakeBody(-direction);
}

void Arena::generateFood()
{
	if (auto pos = Venue::generateFood())
	{
		auto [x, y] = pos.value();
		paintElement(Element::Food, x, y);
	}
}