#pragma once
#ifndef SNAKE_INTERFACE_HEADER_
#define SNAKE_INTERFACE_HEADER_

class NotCopyable
{
protected:
	NotCopyable() = default;
	~NotCopyable() = default;
	NotCopyable(const NotCopyable&) = delete;
	NotCopyable& operator=(const NotCopyable&) = delete;
};

class Interface :NotCopyable
{
public:
	virtual ~Interface() = default;
};

#endif // SNAKE_INTERFACE_HEADER_