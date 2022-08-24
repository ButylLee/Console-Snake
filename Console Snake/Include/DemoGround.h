#pragma once
#ifndef SNAKE_DEMOGROUND_HEADER_
#define SNAKE_DEMOGROUND_HEADER_

#include "Canvas.h"
#include "Arena.h"

class DemoGround
{
public:
	DemoGround(Canvas& canvas);
	DemoGround(const DemoGround&) = delete;
	DemoGround& operator=(const DemoGround&) = delete;

public:
	void show();

private:
	Canvas& canvas;
	Arena arena;
};

#endif // SNAKE_DEMOGROUND_HEADER_