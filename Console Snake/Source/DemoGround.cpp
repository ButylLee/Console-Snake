#include "DemoGround.h"
#include "WideIO.h"

DemoGround::DemoGround(Canvas& canvas)
	: canvas(canvas), arena(canvas)
	, map(arena.getCurrentMap())
{

}

void DemoGround::show()
{
	while (!arena.isOver())
	{

		arena.updateFrame();
	}
}

void DemoGround::solveNextStep()
{

}
