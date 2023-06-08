#include "Arena.h"
#include "GlobalData.h"
#include "WideIO.h"
#include "Random.h"

#include <utility>
#include <type_traits>

Arena::Arena(Canvas& canvas)
	: canvas(canvas), map(GameSetting::get().height, GameSetting::get().width)
	, snake_body((GameSetting::get().height - 2) * (GameSetting::get().width - 2))
{
	setupInvariantAndPaint();
	createSnake();
	createFood();
}

void Arena::updateFrame()
{
	// get new snake head position
	auto [head_x, head_y] = snake_body[snake_head];
	forwardIndex(snake_head);
	Direction key = input_key.exchange(Direction::None);
	if (key != Direction::None && !key.isConflictWith(snake_direct))
		snake_direct = key;
	nextPosition(head_x, head_y);

	// check is dashing against barrier or body
	auto previous_type = map[head_x][head_y].type;
	if (previous_type == Element::barrier || previous_type == Element::snake)
	{
		game_over = true;
		return;
	}

	// process snake head and rebind data
	map[head_x][head_y].type = Element::snake;
	paintElement(Element::snake, head_x, head_y);
	rebindData(snake_head, head_x, head_y);

	// process snake tail
	if (previous_type == Element::food)
	{
		createFood();
		GameData::get().score++;
	}
	else
	{
		auto [tail_x, tail_y] = snake_body[snake_tail];
		forwardIndex(snake_tail);
		map[tail_x][tail_y].type = Element::blank;
		paintElement(Element::blank, tail_x, tail_y);
	}
}

void Arena::paintElement(Element which) noexcept
{
	canvas.setColor(GameSetting::get().theme.Value()[which].color);
	print(GameSetting::get().theme.Value()[which].facade);
}

void Arena::paintElement(Element which, short x, short y) noexcept
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
	return GameData::get().score + snake_begin_length == snake_body.size();
}

SnakeNode Arena::getNextPosition() const noexcept
{
	auto [x, y] = snake_body[snake_head];
	nextPosition(x, y);
	return { x,y };
}

Element Arena::getPositionType(uint8_t x, uint8_t y) const noexcept
{
	return map[x][y].type;
}

const DynArray<MapNode, 2>& Arena::getCurrentMap() const noexcept
{
	return map;
}

void Arena::setupInvariantAndPaint() noexcept
{
	canvas.setCursor(0, 0);
	for (int index = 0, row = 0; row < GameSetting::get().height; row++)
	{
		for (int column = 0; column < GameSetting::get().width; column++)
		{
			if (row == 0 || row == GameSetting::get().height - 1 ||
				column == 0 || column == GameSetting::get().width - 1)
			{
				map[column][row].type = Element::barrier;
				map[column][row].snake_index = -1;
				if (!(GameSetting::get().old_console_host &&
					  row == GameSetting::get().height - 1 &&
					  column == GameSetting::get().width - 1))
					paintElement(Element::barrier);
			}
			else
			{
				map[column][row].type = Element::blank;
				map[column][row].snake_index = index;
				snake_body[index].x = column;
				snake_body[index].y = row;
				paintElement(Element::blank);
				index++;
			}
		}
		if (row != GameSetting::get().height - 1)
		{
			canvas.nextLine();
		}
	}
}

void Arena::createSnake()
{
	// random snake initial postion
	auto x_range = GameSetting::get().width - 2 * (1 + snake_begin_length);
	auto y_range = GameSetting::get().height - 2 * (1 + snake_begin_length);
	auto begin_head_x = GetRandom(0, x_range);
	auto begin_head_y = GetRandom(0, y_range);

	// select direction
	if (bool use_x = GetRandom(0, 1))
	{
		if (begin_head_x < x_range / 2)
			snake_direct = Direction::Right;
		else
			snake_direct = Direction::Left;
	}
	else
	{
		if (begin_head_y < y_range / 2)
			snake_direct = Direction::Down;
		else
			snake_direct = Direction::Up;
	}

	// place initial snake body
	begin_head_x += 1 + snake_begin_length;
	begin_head_y += 1 + snake_begin_length;
	snake_tail = snake_head = map[begin_head_x][begin_head_y].snake_index;
	for (size_t i = 0; i < snake_begin_length; i++)
	{
		map[begin_head_x][begin_head_y].type = Element::snake;
		paintElement(Element::snake, begin_head_x, begin_head_y);
		rebindData(snake_tail, begin_head_x, begin_head_y);

		if (i == snake_begin_length - 1)
			break;
		switch (+snake_direct)
		{
			case Direction::Up:
				begin_head_y++; break;
			case Direction::Down:
				begin_head_y--; break;
			case Direction::Left:
				begin_head_x++; break;
			case Direction::Right:
				begin_head_x--; break;
		}
		snake_tail++;
	}
}

void Arena::createFood()
{
	size_t range, random_index;
	// get usable range for generating food
	if (snake_head > snake_tail)
	{
		range = snake_head - snake_tail - 1;
	}
	else
	{
		range = snake_body.total_size() - (snake_tail - snake_head + 1);
	}
	if (range == 0)
		return;

	// pick random position of food
	random_index = GetRandom(1, range) + snake_tail;
	if (random_index >= snake_body.total_size())
		random_index -= snake_body.total_size();

	// generate food on the map
	auto [x, y] = snake_body[random_index];
	map[x][y].type = Element::food;
	paintElement(Element::food, x, y);
}

void Arena::rebindData(int16_t index, int8_t x, int8_t y) noexcept
{
	// maintain class invariant
	int16_t temp_index = map[x][y].snake_index;
	std::swap(map[snake_body[index].x][snake_body[index].y].snake_index,
			  map[x][y].snake_index);
	std::swap(snake_body[index].x, snake_body[temp_index].x);
	std::swap(snake_body[index].y, snake_body[temp_index].y);
}

void Arena::forwardIndex(int16_t& index) const noexcept
{
	index = index == 0
		? static_cast<int16_t>(snake_body.total_size() - 1)
		: index - 1;
}

void Arena::nextPosition(uint8_t& x, uint8_t& y) const noexcept
{
	switch (+snake_direct)
	{
		case Direction::Up:
			y == 0 ? y = GameSetting::get().height - 1 : y--;
			break;
		case Direction::Down:
			y == GameSetting::get().height - 1 ? y = 0 : y++;
			break;
		case Direction::Left:
			x == 0 ? x = GameSetting::get().width - 1 : x--;
			break;
		case Direction::Right:
			x == GameSetting::get().width - 1 ? x = 0 : x++;
			break;
	}
}