#pragma once
#ifndef SNAKE_ATOMICOPERATION_HEADER_
#define SNAKE_ATOMICOPERATION_HEADER_
#include <atomic>

template<typename T>
constexpr T loadAtomic(const std::atomic<T>& atom) noexcept
{
	return atom.load(std::memory_order_relaxed);
}

template<typename T>
constexpr void storeAtomic(std::atomic<T>& atom, const T value) noexcept
{
	atom.store(value, std::memory_order_relaxed);
}

template<typename T>
constexpr T exchangeAtomic(std::atomic<T>& atom, const T value) noexcept
{
	return atom.exchange(value, std::memory_order_relaxed);
}

#endif // SNAKE_ATOMICOPERATION_HEADER_