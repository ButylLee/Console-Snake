#pragma once
#ifndef SNAKE_DEMOGROUND_HEADER_
#define SNAKE_DEMOGROUND_HEADER_

#include "Interface.h"
#include "Canvas.h"
#include "Arena.h"

class DemoGround :NotCopyable
{
public:
	DemoGround(Canvas& canvas);

public:
	void show();

private:
	void solveNextStep();

private:
	Canvas& canvas;
	Arena arena;

	const DynArray<MapNode, 2>& map;
};

#endif // SNAKE_DEMOGROUND_HEADER_