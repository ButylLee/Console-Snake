#pragma once
#ifndef SNAKE_RANK_HEADER_
#define SNAKE_RANK_HEADER_

#include "Singleton.h"
#include "DataSet.h"
#include <utility>
#include <vector>
#include <string>
#include <future>
#include <mutex>
#include <shared_mutex>
#include <algorithm>
#include <cassert>

class RankBase
{
public:
	struct RankItem {
		std::wstring name;
		int score = 0;
		Speed::value_type speed = {};
		Size::value_type width = {};
		Size::value_type height = {};
		bool is_win = false;
	};
	static constexpr auto rank_count = 10;
	static constexpr auto name_max_length = 12;

protected:
	RankBase() try
	{
		rank_table.reserve(rank_count + 1);
	}
	catch (...)
	{
		// Strong exception guarantee of vector.reserve
	}

public:
	void newResult(std::wstring new_name, int new_score, bool winning)
	{
		if (done.valid()) // wait for last time sorting
			done.get();

		RankItem new_one = {
			std::move(new_name),
			new_score,
			GameSetting::get().speed.Value(),
			GameSetting::get().width,
			GameSetting::get().height,
			winning
		};

		done = std::async(std::launch::async,
						  [this, new_one = std::move(new_one)]
						  {
							  auto [rank, lock] = this->modifyRank();
							  rank.push_back(new_one);

							  auto previous_user = std::find_if(rank.begin(), rank.end(),
																[&](const RankItem& lhs) noexcept
																{
																	return lhs.name == rank.back().name;
																});
							  auto end = rank.end();
							  assert(previous_user != end);
							  // find and store previous named gamer's best score
							  if (previous_user != rank.cend() - 1 && !rank.back().name.empty())
							  {
								  if (rank.back().score >= previous_user->score)
									  *previous_user = std::move(rank.back());
								  --end;
							  }
							  std::stable_sort(rank.begin(), end,
											   [](const RankItem& lhs, const RankItem& rhs) noexcept
											   {
												   return lhs.score > rhs.score;
											   });
							  rank.pop_back();
						  });
	}

	std::pair<const std::vector<RankItem>&, std::shared_lock<std::shared_mutex>>
	getRank() const noexcept
	{
		return { rank_table, std::shared_lock{ rank_mutex } };
	}

	std::pair<std::vector<RankItem>&, std::unique_lock<std::shared_mutex>>
	modifyRank() noexcept
	{
		return { rank_table, std::unique_lock{ rank_mutex } };
	}

	void clearRank() noexcept
	{
		std::unique_lock lock(rank_mutex);
		rank_table.clear();
		rank_table = std::vector<RankItem>{ rank_count };
	}

private:
	std::vector<RankItem> rank_table{ rank_count };
	std::future<void> done;
	mutable std::shared_mutex rank_mutex;
};

using Rank = Singleton<RankBase>;

#endif // SNAKE_RANK_HEADER_