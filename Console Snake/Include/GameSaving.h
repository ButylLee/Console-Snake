#pragma once
#ifndef SNAKE_GAMESAVING_HEADER_
#define SNAKE_GAMESAVING_HEADER_

#include "Singleton.h"
#include "Rank.h"
#include "Resource.h"
#include <cstdint>
#include <future>

class GameSavingBase
{
	struct alignas(8) SettingSavingItem
	{
		int64_t theme[static_cast<size_t>(Element::Mask)] = {};
		int16_t speed = 0;
		int16_t width = 0;
		int16_t height = 0;
		int8_t lang = 0;
		int8_t show_frame = 0;
	};
	struct alignas(8) RankSavingItem
	{
		int16_t score = 0;
		int16_t width = 0;
		int16_t height = 0;
		int8_t speed = 0;
		int8_t is_win = 0;
		wchar_t name[Rank::name_max_length] = {};
	};
protected:
	GameSavingBase();

public:
	void save();
	void convertFromBinaryData() noexcept;

private:
	void convertToBinaryData() noexcept;

private:
	struct alignas(8) {
		SettingSavingItem setting;
		RankSavingItem rank_list[Rank::rank_count];
	}bin_data;

	std::future<void> done;
	bool no_save_file = true;
};

using GameSaving = Singleton<GameSavingBase>;

#endif // SNAKE_GAMESAVING_HEADER_