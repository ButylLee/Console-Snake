#pragma once
#ifndef SNAKE_GAMESAVING_HEADER_
#define SNAKE_GAMESAVING_HEADER_

#include "Modules.h"
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
	struct Magic
	{
		uint16_t number = 0x4c69;
		char version[6] = GAME_VERSION;
	};

	struct SettingSavingItem
	{
		MapCell map[Map::MaxMapSetCount - MapSet::Mask_] = {};
		wchar_t map_name[Map::MaxMapSetCount - MapSet::Mask_][Map::NameMaxHalfWidth] = {};
		int16_t theme[static_cast<size_t>(Element::Mask_)][2] = {};
		int16_t speed = 0;
		int16_t map_size = 0;
		uint8_t custom_map_count = 0;
		uint8_t map_select = 0;
		int8_t lang = 0;
		int8_t show_frame = 0;
		int8_t opening_pause = 0;
		int8_t mute = 0;
	};

	struct RankSavingItem
	{
		wchar_t name[Rank::NameMaxLength] = {};
		wchar_t map_name[Map::NameMaxHalfWidth] = {};
		int16_t score = 0;
		int16_t size = 0;
		int8_t speed = 0;
		int8_t is_win = 0;
	};

protected:
	GameSavingBase();

public:
	void save();
	void convertFromSaveData() noexcept;

private:
	void convertToSaveData() noexcept;

private:
	struct {
		Magic magic;
		SettingSavingItem setting;
		RankSavingItem rank_list[Rank::RankCount];
	}bin_data;

	std::future<void> done;
	bool no_save_file = true;
};

using GameSaving = ModuleRegister<GameSavingBase>;

#endif // SNAKE_GAMESAVING_HEADER_