#pragma once
#ifndef SNAKE_RANDOM_HEADER_
#define SNAKE_RANDOM_HEADER_

#include <random>
#include <concepts>
#include <cassert>

inline auto& GetRandomEngine()
{
	static std::default_random_engine engine(std::random_device{}());
	return engine;
}

inline void ReSeed()
{
	GetRandomEngine().seed(std::random_device{}());
}

// random interval: [min,max]
template<std::integral T>
inline T GetRandom(T min, T max)
{
	static std::uniform_int_distribution<T> dis;
	using param_type = typename decltype(dis)::param_type;
	assert(min <= max);
	return dis(GetRandomEngine(), param_type{ min,max });
}

// random interval: [min,max)
template<std::floating_point T>
inline T GetRandom(T min, T max)
{
	static std::uniform_real_distribution<T> dis;
	using param_type = typename decltype(dis)::param_type;
	assert(min < max);
	return dis(GetRandomEngine(), param_type{ min,max });
}

#endif // SNAKE_RANDOM_HEADER_