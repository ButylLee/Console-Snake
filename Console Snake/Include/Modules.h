#pragma once
#ifndef SNAKE_MODULES_HEADER_
#define SNAKE_MODULES_HEADER_

#include <vector>
#include <ranges>
#include <type_traits>

class ModuleManager
{
	template<typename Base>
	friend class ModuleRegister;
	struct ModuleManageFunc
	{
		using CreatorFunc = void* (*)();
		using DeleterFunc = void(*)(void*) noexcept;
		CreatorFunc creator = nullptr;
		DeleterFunc deleter = nullptr;
	};
public:
	ModuleManager()
	{
		for (auto& func : functions)
		{
			objects.emplace_back(func.creator());
		}
	}
	~ModuleManager() noexcept
	{
		for (auto& func : functions | std::views::reverse)
		{
			func.deleter(objects.back());
			objects.pop_back();
		}
	}

private:
	inline static std::vector<ModuleManageFunc> functions;
	std::vector<void*> objects;
};

template<typename Base>
class ModuleRegister :public Base
{
public:
	ModuleRegister() noexcept(std::is_nothrow_default_constructible_v<Base>)
		:Base()
	{
		if (!instance) instance = this;
	}
	static Base& get() noexcept { return *instance; }

	ModuleRegister(const ModuleRegister&) = delete;
	ModuleRegister& operator=(const ModuleRegister&) = delete;

private:
	inline static ModuleRegister* instance = nullptr;

	template<typename T>
	static void* ModuleNew() { return new T; }
	template<typename T>
	static void ModuleDelete(void* object) noexcept
	{
		delete static_cast<T*>(object);
	}
	inline static bool _is_registered = []
	{
		ModuleManager::functions.push_back(
			{
				ModuleNew<ModuleRegister>,
				ModuleDelete<ModuleRegister>
			}
		);
		return true;
	}();
};

#endif // SNAKE_MODULES_HEADER_