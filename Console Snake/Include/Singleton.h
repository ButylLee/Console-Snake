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
	static std::unique_ptr<Singleton> instance;
};
template<typename Base>
inline std::unique_ptr<Singleton<Base>> Singleton<Base>::instance{ NewWithHandler<Singleton<Base>>() };

#endif // SNAKE_SINGLETON_HEADER_