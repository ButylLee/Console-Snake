﻿#include "Arena.h"
#include "GlobalData.h"
#include "WideIO.h"
#include "Random.h"
#include "ErrorHandling.h"
#include "Pythonic.h"
#include "SoundPlayer.h"

#include <utility>
#include <algorithm>
#include <vector>
#include <iterator>
#include <ranges>
#include <cassert>
#include <cmath>

namespace
{
	DynArray<MapNode, 2> GetMap()
	{
		DynArray<MapNode, 2> map(GameSetting::get().map.size.Value(),
								 GameSetting::get().map.size.Value());
		auto f = [&](const auto& m)
			{
				assert(map.total_size() == m.size());
				std::transform(m.begin(), m.end(), map.iter_all().begin(),
							   [](Element type)
							   {
								   MapNode node{ .type = type };
								   return node;
							   });
			};
		GameSetting::get().map.visitValue(f);
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

Venue::Venue(DynArray<MapNode, 2> map_)
	: map(std::move(map_)), snake_body(GetMapBlankCount(map))
{
	setupInvariant();
	createSnake();
	generateFood();
}

PosNode Venue::getNextPosition() const noexcept
{
	auto [x, y] = snake_body[snake_head_index];
	nextPosition(x, y, snake_direct);
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

std::optional<PosNode> Venue::generateFood()
{
	size_t range;
	// get usable range for food generating
	if (snake_head_index > snake_tail_index)
		range = snake_head_index - snake_tail_index - 1;
	else
		range = snake_body.total_size() - (snake_tail_index - snake_head_index + 1);
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
	nextPosition(head_x, head_y, snake_direct);

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

bool Venue::isWin(size_t score) const noexcept
{
	return score + snake_init_length == snake_body.size();
}

void Venue::setupInvariant() noexcept
{
	int16_t index = 0;
	for (auto row : range<uint8_t>(map.size(0)))
	{
		for (auto column : range<uint8_t>(map.size(1)))
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

namespace
{
	struct BlankNodeGenInfo
	{
		struct PosOffset
		{
			int8_t x;
			int8_t y;
			friend PosNode OffsetPosManaged(PosNode pos, PosOffset offset, size_t size_y_, size_t size_x_) noexcept
			{
				int8_t x = pos.x + offset.x;
				int8_t y = pos.y + offset.y;
				int8_t size_x = static_cast<int8_t>(size_x_);
				int8_t size_y = static_cast<int8_t>(size_y_);
				if (x < 0) x += size_x;
				else if (x >= size_x) x -= size_x;
				if (y < 0) y += size_y;
				else if (y >= size_y) y -= size_y;
				pos.x = x;
				pos.y = y;
				return pos;
			}
		};
		static constexpr PosOffset GenConvolutionOffset[24] =
		{
			{ -2,-2 }, { -1,-2 }, { +0,-2 }, { +1,-2 }, { +2,-2 },
			{ -2,-1 }, { -1,-1 }, { +0,-1 }, { +1,-1 }, { +2,-1 },
			{ -2,+0 }, { -1,+0 },            { +1,+0 }, { +2,+0 },
			{ -2,+1 }, { -1,+1 }, { +0,+1 }, { +1,+1 }, { +2,+1 },
			{ -2,+2 }, { -1,+2 }, { +0,+2 }, { +1,+2 }, { +2,+2 },
		};
		static constexpr size_t InitDirectCount = 4;
		static constexpr Direction::Tags InitDirectMap[InitDirectCount] =
		{ Direction::Up, Direction::Left, Direction::Right, Direction::Down };
		static constexpr PosOffset InitDirectCalcOffset1[InitDirectCount] =
		{ { 0,+1 }, { +1,0 }, { -1,0 }, { 0,-1 } };
		static constexpr PosOffset InitDirectCalcOffset2[InitDirectCount] =
		{ { 0,+2 }, { +2,0 }, { -2,0 }, { 0,-2 } };

		PosNode pos;
		int gen_probability = 1;
		int init_direct_probability[InitDirectCount] = {};
	};
	constexpr auto ProbabilityNonlinearizer1 = [](auto x) { return x * x; };
	constexpr auto ProbabilityNonlinearizer2 = [](auto x) { return static_cast<decltype(x)>(std::pow(10, x)); };

	struct SquareMapInfo
	{
		uint8_t margin_up = 0;
		uint8_t margin_down = 0;
		uint8_t margin_left = 0;
		uint8_t margin_right = 0;
	};
	std::optional<SquareMapInfo> IsSquareMap(const DynArray<MapNode, 2>& map) noexcept
	{
		auto g = [](auto map_slice, auto inner_range) -> bool
			{
				for (auto i : inner_range)
					if (map_slice(i).type == Element::Blank)
						return true;
				return false;
			};
		auto f = [&](auto slice_functor, auto outer_range, auto inner_range, auto& counter)
			{
				for (auto i : outer_range)
				{
					if (g(slice_functor(i), inner_range))
						break;
					counter++;
				}
			};
		auto slice_y = [&](auto y) { return [&, y](auto x) { return map[y][x]; }; };
		auto slice_x = [&](auto x) { return [&, x](auto y) { return map[y][x]; }; };

		SquareMapInfo info;
		f(slice_y, range(map.size(0)), range(map.size(1)), info.margin_up);
		f(slice_y, range(map.size(0)) | std::views::reverse, range(map.size(1)), info.margin_down);
		f(slice_x, range(map.size(1)), range(map.size(0)), info.margin_left);
		f(slice_x, range(map.size(1)) | std::views::reverse, range(map.size(0)), info.margin_right);

		for (auto y : range(info.margin_up, map.size(0) - info.margin_down))
		{
			for (auto x : range(info.margin_left, map.size(1) - info.margin_right))
			{
				if (map[y][x].type != Element::Blank)
					return {};
			}
		}
		if (info.margin_up + info.margin_down + info.margin_left + info.margin_right == 0)
			return {};
		return info;
	}
}

void Venue::createSnake()
{
	Direction init_direction;
	uint8_t pos_x, pos_y;

	if (auto square_info = IsSquareMap(map); !square_info.has_value()) // general algorithm
	{
		std::vector<BlankNodeGenInfo> blank_list;
		for (auto y : range<uint8_t>(map.size(0)))
		{
			for (auto x : range<uint8_t>(map.size(1)))
			{
				auto& node = map[y][x];
				if (node.type != Element::Blank)
					continue;
				BlankNodeGenInfo info;
				// record Blank position
				info.pos.x = x;
				info.pos.y = y;

				// calculate generate probability
				for (auto i : range(std::size(info.GenConvolutionOffset)))
				{
					auto offset = info.GenConvolutionOffset[i];
					auto curr_pos = OffsetPosManaged(info.pos, offset, map.size(0), map.size(1));
					if (map[curr_pos.y][curr_pos.x].type == Element::Blank)
						info.gen_probability++;
				}
				info.gen_probability = ProbabilityNonlinearizer1(info.gen_probability);

				// calculate initial direction probability
				for (auto i : range(std::size(info.init_direct_probability)))
				{
					auto offset1 = info.InitDirectCalcOffset1[i];
					auto offset2 = info.InitDirectCalcOffset2[i];
					auto curr_pos1 = OffsetPosManaged(info.pos, offset1, map.size(0), map.size(1));
					auto curr_pos2 = OffsetPosManaged(info.pos, offset2, map.size(0), map.size(1));
					if (map[curr_pos1.y][curr_pos1.x].type == Element::Blank)
						info.init_direct_probability[i]++;
					if (map[curr_pos2.y][curr_pos2.x].type == Element::Blank)
						info.init_direct_probability[i]++;
				}
				std::transform(std::begin(info.init_direct_probability),
							   std::end(info.init_direct_probability),
							   std::begin(info.init_direct_probability),
							   ProbabilityNonlinearizer2);

				blank_list.push_back(std::move(info));
			}
		}
		if (blank_list.size() == 0)
			throw RuntimeException(L"Invalid Map.");

		auto fn = [&](auto i) { return blank_list[static_cast<size_t>(i)].gen_probability; };
		auto& init_node = blank_list[GetWeightedDiscreteRandom<size_t>(blank_list.size(), fn)];
		init_direction = BlankNodeGenInfo::InitDirectMap[
			GetWeightedDiscreteRandom<size_t>(std::begin(init_node.init_direct_probability),
											  std::end(init_node.init_direct_probability)
			)
		];
		pos_x = init_node.pos.x;
		pos_y = init_node.pos.y;
	}
	else // specialized algorithm for square-type maps
	{
		auto y_range = map.size(0) - square_info->margin_up - square_info->margin_down;
		auto x_range = map.size(1) - square_info->margin_left - square_info->margin_right;
		if (y_range > map.size(0) || x_range > map.size(1))
			throw RuntimeException(L"Invalid Map.");
		pos_y = static_cast<uint8_t>(GetRandom(0, y_range - 1));
		pos_x = static_cast<uint8_t>(GetRandom(0, x_range - 1));

		if (bool select_x_axis = GetRandom(0, 1))
		{
			if (pos_x < x_range / 2)
				init_direction = Direction::Right;
			else
				init_direction = Direction::Left;
		}
		else
		{
			if (pos_y < y_range / 2)
				init_direction = Direction::Down;
			else
				init_direction = Direction::Up;
		}
		pos_x += square_info->margin_left;
		pos_y += square_info->margin_up;
	}

	addSnakeBody(init_direction, pos_x, pos_y);
	for (auto _ : range(SnakeIntendedInitLength - 1))
		addSnakeBody(-init_direction);
}

void Venue::addSnakeBody(Direction head_direct, uint8_t head_x, uint8_t head_y) noexcept
{
	assert(snake_head_index == -1 && snake_tail_index == -1 &&
		   snake_direct == Direction::None);
	snake_direct = head_direct;
	assert(map[head_y][head_x].type == Element::Blank);
	map[head_y][head_x].type = Element::Snake;
	snake_tail_index = snake_head_index = map[head_y][head_x].snake_index;
	rebindData(snake_head_index, head_x, head_y);
	snake_init_length++;
}

void Venue::addSnakeBody(Direction tail_direct) noexcept
{
	assert(snake_head_index != -1 && snake_tail_index != -1);
	if (tail_direct == snake_direct)
		return;
	auto [tail_x, tail_y] = snake_body[snake_tail_index];
	nextPosition(tail_x, tail_y, tail_direct);
	if (map[tail_y][tail_x].type != Element::Blank)
		return;
	map[tail_y][tail_x].type = Element::Snake;
	backwardIndex(snake_tail_index);
	rebindData(snake_tail_index, tail_x, tail_y);
	snake_init_length++;
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

void Venue::nextPosition(uint8_t& x, uint8_t& y, Direction direct) const noexcept
{
	uint8_t height = static_cast<uint8_t>(map.size(0));
	uint8_t width = static_cast<uint8_t>(map.size(1));
	switch (+direct)
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
	: Venue(GetMap()), canvas(canvas)
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
			SoundPlayer::get().play(isWin() ? Sounds::Win : Sounds::Dead);
			break;
		case 1: // food
			paintElement(Element::Snake, nodes_updated.head_pos.x, nodes_updated.head_pos.y);
			generateFood();
			GameData::get().score++;
			SoundPlayer::get().play(Sounds::Food);
			break;
		case 2: // move normally
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
	return Venue::isWin(GameData::get().score);
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
	for (auto&& [index, node] : enumerate(map.iter_all()))
	{
		if (not(GameSetting::get().old_console_host && index == map.total_size() - 1))
			paintElement(node.type);
	}
}

void Arena::generateFood()
{
	if (auto pos = Venue::generateFood())
	{
		auto [x, y] = pos.value();
		paintElement(Element::Food, x, y);
	}
}