#pragma once
#ifndef SNAKE_TIMER_HEADER_
#define SNAKE_TIMER_HEADER_

#include <utility>
#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <concepts>

class Timer
{
public:
	static constexpr std::chrono::milliseconds minimum_interval{ 1 };
	static constexpr enum class _Loop { _ } Loop{};

public:
	template<std::invocable F, typename Rep, typename Period>
	Timer(F&& f, std::chrono::duration<Rep, Period> delay, _Loop loop = (_Loop)1)
	{
		if (loop == Loop)
			timer_loop = true;
		std::thread(
			[this, timer_enable = timer_enable, f = std::move(f), delay]
			{
				using namespace std::chrono;
				do {
					auto end = high_resolution_clock::now() + delay;
					do {
						std::this_thread::sleep_for(minimum_interval);
						if (!*timer_enable)
							return;
						if (timer_reset)
						{
							timer_reset = false;
							end = high_resolution_clock::now() + delay;
						}
					} while (high_resolution_clock::now() < end);

					if (*timer_enable)
						f();
					else
						return;
				} while (timer_loop);
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
		timer_reset = true;
	}

	void loop(bool looping) noexcept
	{
		timer_loop = looping;
	}

private:
	std::shared_ptr<bool> timer_enable = std::make_shared<bool>(true);
	std::atomic<bool> timer_reset = false;
	std::atomic<bool> timer_loop = false;
};

#endif // SNAKE_TIMER_HEADER_