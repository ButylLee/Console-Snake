#include "Rank.h"
#include <algorithm>
#include <cassert>

RankBase::RankBase() noexcept
{
	try {
		rank_table.reserve(rank_count + 1);
	}
	catch (...) {
		// Strong exception guarantee of vector.reserve(...)
	}
}

void RankBase::newResult(std::wstring new_name, int new_score, bool winning)
{
	if (done.valid()) // wait for last time sorting
		done.get();

	RankItem new_one = {
		std::move(new_name),
		new_score,
		GameSetting::get().speed.Value(),
		GameSetting::get().map.set.Name(),
		GameSetting::get().map.size.Value(),
		winning
	};

	done = std::async(std::launch::async,
					  [this, new_one = std::move(new_one)]() mutable
					  {
						  auto [rank, lock] = this->modifyRank();
						  rank.push_back(std::move(new_one));

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

std::pair<const std::vector<RankBase::RankItem>&, std::shared_lock<std::shared_mutex>>
RankBase::getRank() const
{
	return { rank_table, std::shared_lock{ rank_mutex } };
}

std::pair<std::vector<RankBase::RankItem>&, std::unique_lock<std::shared_mutex>>
RankBase::modifyRank()
{
	return { rank_table, std::unique_lock{ rank_mutex } };
}

void RankBase::clearRank()
{
	std::unique_lock lock(rank_mutex);
	rank_table.clear();
	rank_table = std::vector<RankItem>{ rank_count };
}