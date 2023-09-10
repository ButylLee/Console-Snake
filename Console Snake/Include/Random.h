#pragma once
#ifndef SNAKE_RANDOM_HEADER_
#define SNAKE_RANDOM_HEADER_

#include <random>
#include <concepts>
#include <cassert>
#include <type_traits>

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
template<std::integral T1, std::integral T2>
inline std::common_type_t<T1, T2> GetRandom(T1 min, T2 max)
{
	using int_type = std::common_type_t<T1, T2>;
	static std::uniform_int_distribution<int_type> dis;
	using param_type = typename decltype(dis)::param_type;
	assert(static_cast<int_type>(min) <= static_cast<int_type>(max));
	return dis(GetRandomEngine(),
			   param_type{ static_cast<int_type>(min), static_cast<int_type>(max) });
}

// random interval: [min,max)
template<std::floating_point T1, std::floating_point T2>
inline std::common_type_t<T1, T2> GetRandom(T1 min, T2 max)
{
	using float_type = std::common_type_t<T1, T2>;
	static std::uniform_real_distribution<float_type> dis;
	using param_type = typename decltype(dis)::param_type;
	assert(static_cast<float_type>(min) < static_cast<float_type>(max));
	return dis(GetRandomEngine(),
			   param_type{ static_cast<float_type>(min), static_cast<float_type>(max) });
}

// random interval: [0, n)
template<std::integral T = int, typename Iter>
inline T GetWeightedDiscreteRandom(Iter first, Iter last)
{
	static std::discrete_distribution<T> dis;
	using param_type = typename decltype(dis)::param_type;
	assert(first <= last);
	return dis(GetRandomEngine(),
			   param_type{ first, last });
}

// random interval: [0, count)
// Fn: (i:int)->probability:int
template<std::integral T = int, typename Fn>
	requires requires(Fn f) { { f(1) } -> std::integral; }
inline T GetWeightedDiscreteRandom(size_t count, Fn fn)
{
	static std::discrete_distribution<T> dis;
	using param_type = typename decltype(dis)::param_type;
	return dis(GetRandomEngine(),
			   param_type{ count, 0, static_cast<double>(count), fn });
}

#endif // SNAKE_RANDOM_HEADER_