#pragma once
#ifndef SNAKE_GAMESAVING_HEADER_
#define SNAKE_GAMESAVING_HEADER_

#include "Singleton.h"
#include "Rank.h"
#include "Resource.h"
#include <cstdint>
#include <future>

/*
 * GameSaving object: saving and loading save file
 *
 * The data flow:
 *     Game Data Structure ->   Fixed Width Data
 *                                     v
 *        Save File Data   <- Encrypted Binary Data
 */

class GameSavingBase
{
	struct alignas(8) SettingSavingItem
	{
		int16_t theme[static_cast<size_t>(Element::Mask)][2] = {};
		int16_t speed = 0;
		int16_t width = 0;
		int16_t height = 0;
		int8_t lang = 0;
		int8_t show_frame = 0;
	};
	static_assert(sizeof(SettingSavingItem) == 8 + 4 * (size_t)Element::Mask, "sizeof SettingSavingItem is incorrect.");

	struct alignas(8) RankSavingItem
	{
		int16_t score = 0;
		int16_t width = 0;
		int16_t height = 0;
		int8_t speed = 0;
		int8_t is_win = 0;
		wchar_t name[Rank::name_max_length] = {};
	};
	static_assert(sizeof(RankSavingItem) == 8 + 2 * Rank::name_max_length, "sizeof RankSavingItem is incorrect.");

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
	static_assert(sizeof(bin_data) == sizeof(SettingSavingItem) + sizeof(RankSavingItem) * Rank::rank_count, "sizeof bin_data is incorrect.");

	std::future<void> done;
	bool no_save_file = true;
};

using GameSaving = Singleton<GameSavingBase>;

#endif // SNAKE_GAMESAVING_HEADER_