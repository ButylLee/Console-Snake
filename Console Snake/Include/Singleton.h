#pragma once
#ifndef SNAKE_SINGLETON_HEADER_
#define SNAKE_SINGLETON_HEADER_

#include "ErrorHandling.h"
#include <memory>

// TEMPLATE Singleton
template<typename Base>
class Singleton :public Base
{
	template<typename T>
	friend T* NewWithHandler() noexcept;
public:
	static Base& get() noexcept { return *instance; }
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
private:
	Singleton() = default;
	inline static std::unique_ptr<Singleton> instance{ NewWithHandler<Singleton>() };
};

#endif // SNAKE_SINGLETON_HEADER_