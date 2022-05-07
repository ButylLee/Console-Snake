#include "Playground.h"
#include "Rank.h"
#include "Console.h"

#include "wideIO.h"
#include "KeyMap.h"
#include "ScopeGuard.h"

#include <random>
#include <thread>
#include <chrono>
#include <utility>
#include <type_traits>
#include <string>
#include <cwctype>
#include <cassert>
#include "WinMacro.h"
#include <Windows.h>

void Playground::play()
{
	using namespace std::chrono_literals;

	std::thread th_input(
		[this]
		{
			while (true)
			{
				if (game_over)
					return;
				if (loadAtomic(input_key) != Direction::None)
					continue;
				switch (getwch())
				{
					case K_UP: case K_W: case K_w:
						if (loadAtomic(game_status) == GameStatus::Running &&
							loadAtomic(snake_direct) != Direction::Down)
						{
							storeAtomic(input_key, Direction::Up);
						}
						break;

					case K_DOWN: case K_S: case K_s:
						if (loadAtomic(game_status) == GameStatus::Running &&
							loadAtomic(snake_direct) != Direction::Up)
						{
							storeAtomic(input_key, Direction::Down);
						}
						break;

					case K_LEFT: case K_A: case K_a:
						if (loadAtomic(game_status) == GameStatus::Running &&
							loadAtomic(snake_direct) != Direction::Right)
						{
							storeAtomic(input_key, Direction::Left);
						}
						break;

					case K_RIGHT: case K_D: case K_d:
						if (loadAtomic(game_status) == GameStatus::Running &&
							loadAtomic(snake_direct) != Direction::Left)
						{
							storeAtomic(input_key, Direction::Right);
						}
						break;

					case K_Space:
						if (loadAtomic(game_status) == GameStatus::Pausing)
						{
							storeAtomic(game_status, GameStatus::Running);
							if (GameSetting::get().show_frame)
								Console::get().setTitle(~token::title_gaming);
						}
						else
						{
							storeAtomic(game_status, GameStatus::Pausing);
							if (GameSetting::get().show_frame)
								Console::get().setTitle(~token::title_pausing);
						}
						break;

					case K_Esc:
						storeAtomic(game_status, GameStatus::Ending);
						return;
				}
			}
		});
	finally {
		if (th_input.joinable())
			th_input.join();
	};

	while (true)
	{
		switch (loadAtomic(game_status))
		{
			case GameStatus::Running:
				updateFrame();
				if (game_over)
				{
					// tell thread th_input to end
					PostMessageW(Console::get().getConsoleHandle(), WM_KEYDOWN, VK_ESCAPE, 0);
					endGame();
					return;
				}
				else
				{
					std::this_thread::sleep_for(30ms + // 30ms - 210ms, level 1-10
												20ms * (10 - GameSetting::get().speed.Value()));
				}
				break;
			case GameStatus::Pausing:
				break;
			case GameStatus::Ending:
				return;
		}
	}
}

void Playground::paintElement(Element which) noexcept
{
	canvas.setColor(GameSetting::get().theme.Value()[which].color);
	print(GameSetting::get().theme.Value()[which].facade);
}

size_t Playground::getSnakeBodySize() noexcept
{
	return (GameSetting::get().height - 2) * (GameSetting::get().width - 2);
}

void Playground::setupInvariantAndPaint() noexcept
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
				if (!(GameSetting::get().old_console_host == true &&
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
			if (GameSetting::get().old_console_host == false)
				print(L'\n');
		}
	}
}

void Playground::createSnake()
{
	// random snake initial postion
	auto x_range = GameSetting::get().width - 2 * (1 + snake_begin_length);
	auto y_range = GameSetting::get().height - 2 * (1 + snake_begin_length);
	auto begin_head_x = getRandom(0, x_range);
	auto begin_head_y = getRandom(0, y_range);

	// select direction
	if (bool use_x = getRandom(0, 1))
	{
		if (begin_head_x < x_range / 2)
			storeAtomic(snake_direct, Direction::Right);
		else
			storeAtomic(snake_direct, Direction::Left);
	}
	else
	{
		if (begin_head_y < y_range / 2)
			storeAtomic(snake_direct, Direction::Down);
		else
			storeAtomic(snake_direct, Direction::Up);
	}

	// place initial snake body
	begin_head_x += 1 + snake_begin_length;
	begin_head_y += 1 + snake_begin_length;
	snake_tail = snake_head = map[begin_head_x][begin_head_y].snake_index;
	for (size_t i = 0; i < snake_begin_length; i++)
	{
		map[begin_head_x][begin_head_y].type = Element::snake;
		canvas.setCursor(begin_head_x, begin_head_y);
		paintElement(Element::snake);
		rebindData(snake_tail, begin_head_x, begin_head_y);

		if (i == snake_begin_length - 1)
			break;
		switch (loadAtomic(snake_direct))
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

void Playground::createFood()
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
	random_index = getRandom({ 1 }, range) + snake_tail;
	if (random_index >= snake_body.total_size())
		random_index -= snake_body.total_size();

	// generate food on the map
	auto [x, y] = snake_body[random_index];
	map[x][y].type = Element::food;
	canvas.setCursor(x, y);
	paintElement(Element::food);
}

auto& Playground::getRandomEngine()
{
	static std::minstd_rand engine(std::random_device{}());
	return engine;
}

// random interval: [min,max]
template<std::integral T>
T Playground::getRandom(T min, T max)
{
	static std::uniform_int_distribution<T> dis;
	using param_type = typename decltype(dis)::param_type;
	assert(min <= max);
	return dis(getRandomEngine(), param_type{ min,max });
}

void Playground::updateFrame()
{
	// get new snake head position
	auto [head_x, head_y] = snake_body[snake_head];
	forwardIndex(snake_head);
	if (loadAtomic(input_key) != Direction::None)
		storeAtomic(snake_direct, exchangeAtomic(input_key, Direction::None));
	switch (loadAtomic(snake_direct))
	{
		case Direction::Up:
			head_y == 0 ? head_y = GameSetting::get().height - 1 : head_y--;
			break;
		case Direction::Down:
			head_y == GameSetting::get().height - 1 ? head_y = 0 : head_y++;
			break;
		case Direction::Left:
			head_x == 0 ? head_x = GameSetting::get().width - 1 : head_x--;
			break;
		case Direction::Right:
			head_x == GameSetting::get().width - 1 ? head_x = 0 : head_x++;
			break;
	}

	// check is dashing againest barrier or body
	auto previous_type = map[head_x][head_y].type;
	if (previous_type == Element::barrier || previous_type == Element::snake)
	{
		game_over = true;
		return;
	}

	// process snake head and rebind data
	map[head_x][head_y].type = Element::snake;
	canvas.setCursor(head_x, head_y);
	paintElement(Element::snake);
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
		canvas.setCursor(tail_x, tail_y);
		paintElement(Element::blank);
	}
}

void Playground::rebindData(int16_t index, int8_t x, int8_t y) noexcept
{
	// maintain class invariant
	int16_t temp_index = map[x][y].snake_index;
	std::swap(map[snake_body[index].x][snake_body[index].y].snake_index,
			  map[x][y].snake_index);
	std::swap(snake_body[index].x, snake_body[temp_index].x);
	std::swap(snake_body[index].y, snake_body[temp_index].y);
}

void Playground::forwardIndex(int16_t& index) noexcept
{
	index = index == 0
		? static_cast<int16_t>(snake_body.total_size() - 1)
		: index - 1;
}

void Playground::endGame()
{
	auto [baseX, baseY] = canvas.getClientSize();
	baseY = baseY / 2 - 5;
	std::wstring buffer;
	bool is_win = false;

	// show gameover info
	if (GameData::get().score + snake_begin_length == getSnakeBodySize())
	{
		is_win = true;
		Console::get().setTitle(~token::game_congratulations);
		canvas.setColor(Color::Green);

		canvas.setCenteredCursor(~token::game_you_win, baseY);
		print(~token::game_you_win);
	}
	else
	{
		canvas.setColor(Color::LightWhite);

		canvas.setCenteredCursor(~token::game_you_died, baseY);
		print(~token::game_you_died);
	}
	buffer = ~token::game_show_score;
	buffer += std::to_wstring(GameData::get().score);
	canvas.setCenteredCursor(buffer, baseY + 1);
	print(buffer);

	// show info and get gamer's name
	if (GameData::get().score != 0)
	{
		canvas.setColor(Color::Green);
		canvas.setCenteredCursor(~token::game_enter_your_name, baseY + 3);
		print(~token::game_enter_your_name);

		canvas.setColor(Color::LightAqua);
		std::wstring name;
		for (wchar_t ch;;)
		{
			ch = getwchar();
			if (ch == L'\n')
				break;
			if (iswprint(ch))
				name += ch;
		}
		if (name.find_first_not_of(L' ') == std::string::npos)
			name.clear(); // clear if only has spaces
		if (name.length() > Rank::name_max_length)
			name.resize(Rank::name_max_length);
		Rank::get().newResult(name, GameData::get().score, is_win);
	}

	// show Retry Or Return info
	canvas.setColor(Color::LightWhite);
	canvas.setCenteredCursor(~token::game_Space_to_retry, baseY + 6);
	print(~token::game_Space_to_retry);

	canvas.setCenteredCursor(~token::game_Esc_to_return, baseY + 7);
	print(~token::game_Esc_to_return);

	while (true)
	{
		switch (getwch())
		{
			case K_Space:
				GameData::get().retry_game = true;
				[[fallthrough]];
			case K_Esc:
				return;
		}
	}
}