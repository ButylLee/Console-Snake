#pragma once
#ifndef SNAKE_TIMER_HEADER_
#define SNAKE_TIMER_HEADER_

#include "ScopeGuard.h"
#include <utility>
#include <chrono>
#include <thread>
#include <memory>
#include <concepts>

class Timer
{
	static constexpr void NoCallback() noexcept {}
public:
	static constexpr std::chrono::milliseconds minimum_interval{ 1 };
	enum Looping :bool { NoLoop = false, Loop = true };

public:
	template<std::invocable F, typename Rep, typename Period, std::invocable Callback = decltype(NoCallback)>
	Timer(F&& f, std::chrono::duration<Rep, Period> delay,
		  Looping looping = NoLoop, Callback&& callback = NoCallback)
	{
		control->timer_loop = looping;
		std::thread(
			[=, control = control, f = std::move(f), callback = std::move(callback)]() noexcept
			{
				using namespace std::chrono;
				finally {
					callback();
				};
				do {
					auto end = high_resolution_clock::now() + delay;
					do {
						std::this_thread::sleep_for(minimum_interval);
						if (!control->timer_enable)
							return;
						if (control->timer_reset)
						{
							control->timer_reset = false;
							end = high_resolution_clock::now() + delay;
						}
					} while (high_resolution_clock::now() < end);

					if (control->timer_enable)
						f();
					else
						[[unlikely]] return;
				} while (control->timer_loop);
			}).detach();
	}

	~Timer() noexcept
	{
		disable();
	}

	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;

public:
	void disable() noexcept
	{
		control->timer_enable = false;
	}

	void reset() noexcept
	{
		control->timer_reset = true;
	}

	void loop(bool looping) noexcept
	{
		control->timer_loop = looping;
	}

private:
	struct Control {
		bool timer_enable = true;
		bool timer_reset = false;
		bool timer_loop = false;
	};
	std::shared_ptr<Control> control = std::make_shared<Control>();
};

#endif // SNAKE_TIMER_HEADER_