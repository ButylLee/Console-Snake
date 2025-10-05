#pragma once
#ifndef SNAKE_GLOBALRESOURCEWRAPPER_HEADER_
#define SNAKE_GLOBALRESOURCEWRAPPER_HEADER_

#include <type_traits>

template<typename TBase>
class GlobalResourceWrapper :public TBase
{
public:
	using Base = TBase;

public:
	GlobalResourceWrapper() noexcept(std::is_nothrow_default_constructible_v<Base>)
		:Base()
	{
		if (!instance) instance = this;
	}

	static GlobalResourceWrapper& get() noexcept { return *instance; }

private:
	static inline GlobalResourceWrapper* instance = nullptr;
};

#endif // SNAKE_GLOBALRESOURCEWRAPPER_HEADER_