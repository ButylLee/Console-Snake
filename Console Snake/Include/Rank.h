#pragma once
#ifndef SNAKE_RANK_HEADER_
#define SNAKE_RANK_HEADER_

#include "Modules.h"
#include "GlobalData.h"
#include <utility>
#include <vector>
#include <string>
#include <future>
#include <mutex>
#include <shared_mutex>

class RankBase
{
public:
	struct RankItem {
		std::wstring name;
		int score = 0;
		Speed::ValueType speed = {};
		MapSet::NameType map_name;
		Size::ValueType size = {};
		bool is_win = false;
	};
	static constexpr size_t rank_count = 10;
	static constexpr size_t name_max_length = 12;

protected:
	RankBase() noexcept;

public:
	void newResult(std::wstring new_name, int new_score, bool winning);

	std::pair<const std::vector<RankItem>&, std::shared_lock<std::shared_mutex>>
	getRank() const;

	std::pair<std::vector<RankItem>&, std::unique_lock<std::shared_mutex>>
	modifyRank();

	void clearRank();

private:
	std::vector<RankItem> rank_table{ rank_count };
	mutable std::future<void> done;
	mutable std::shared_mutex rank_mutex;
};

using Rank = ModuleRegister<RankBase>;

#endif // SNAKE_RANK_HEADER_