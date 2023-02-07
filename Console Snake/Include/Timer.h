#pragma once
#ifndef SNAKE_TIMER_HEADER_
#define SNAKE_TIMER_HEADER_

#include "ScopeGuard.h"
#include <utility>
#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <concepts>

class Timer
{
	static constexpr void nocallback() noexcept {}
public:
	static constexpr std::chrono::milliseconds minimum_interval{ 1 };
	enum Looping { NoLoop = false, Loop = true };

public:
	template<std::invocable F, typename Rep, typename Period, std::invocable Callback = decltype(nocallback)>
	Timer(F&& f, std::chrono::duration<Rep, Period> delay,
		  Looping loop = NoLoop, Callback&& callback = nocallback)
	{
		*timer_loop = loop;
		std::thread(
			[=, *this, f = std::move(f), callback = std::move(callback)]
			{
				using namespace std::chrono;
				finally {
					callback();
				};
				do {
					auto end = high_resolution_clock::now() + delay;
					do {
						std::this_thread::sleep_for(minimum_interval);
						if (!*timer_enable)
							return;
						if (*timer_reset)
						{
							*timer_reset = false;
							end = high_resolution_clock::now() + delay;
						}
					} while (high_resolution_clock::now() < end);

					if (*timer_enable)
						f();
					else
						[[unlikely]] return;
				} while (*timer_loop);
			}).detach();
	}

	~Timer() noexcept
	{
		*timer_enable = false;
	}

public:
	void disable() noexcept
	{
		*timer_enable = false;
	}

	void reset() noexcept
	{
		*timer_reset = true;
	}

	void loop(bool looping) noexcept
	{
		*timer_loop = looping;
	}

private:
	std::shared_ptr<bool> timer_enable = std::make_shared<bool>(true);
	std::shared_ptr<bool> timer_reset = std::make_shared<bool>(false);
	std::shared_ptr<bool> timer_loop = std::make_shared<bool>(false);
};

#endif // SNAKE_TIMER_HEADER_