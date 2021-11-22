#pragma once
#ifndef SNAKE_RANK_HEADER_
#define SNAKE_RANK_HEADER_

#include "Singleton.h"
#include "DataSet.h"
#include <utility>
#include <vector>
#include <string>
#include <future>
#include <atomic>
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
		this->lock();

		rank_table.push_back(
			{
				std::move(new_name),
				new_score,
				GameSetting::get().speed.Value(),
				GameSetting::get().width,
				GameSetting::get().height,
				winning
			}
		);

		done = std::async(std::launch::async,
						  [this]
						  {
							  auto previous_user = std::find_if(rank_table.begin(), rank_table.end(),
																[this](const RankItem& lhs) noexcept
																{
																	return lhs.name == rank_table.back().name;
																});
							  auto end = rank_table.end();
							  assert(previous_user != end);
							  // find and store previous named gamer's best score
							  if (previous_user != rank_table.cend() - 1 && !rank_table.back().name.empty())
							  {
								  if (rank_table.back().score >= previous_user->score)
									  *previous_user = std::move(rank_table.back());
								  --end;
							  }
							  std::stable_sort(rank_table.begin(), end,
											   [](const RankItem& lhs, const RankItem& rhs) noexcept
											   {
												   return lhs.score > rhs.score;
											   });
							  rank_table.pop_back();
							  this->unlock();
						  });
	}

	void lock() noexcept
	{
		while (rank_lock == true);
		rank_lock = true;
	}

	void unlock() noexcept
	{
		rank_lock = false;
	}

	bool finished() const noexcept
	{
		return !rank_lock;
	}

	const auto& getRank() const noexcept
	{
		while (!finished());
		return rank_table;
	}

	auto& getRank() noexcept
	{
		while (!finished());
		return rank_table;
	}

	auto& getRank_NoLock() noexcept
	{
		return rank_table;
	}

	void clearRank() noexcept
	{
		while (!finished());
		rank_table.clear();
		rank_table = std::vector<RankItem>{ rank_count };
	}

private:
	std::vector<RankItem> rank_table{ rank_count };
	std::future<void> done;
	std::atomic<bool> rank_lock = false;
};

using Rank = Singleton<RankBase>;

#endif // SNAKE_RANK_HEADER_