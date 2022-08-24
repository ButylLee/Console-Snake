#include "DemoGround.h"
#include "wideIO.h"

DemoGround::DemoGround(Canvas& canvas) :canvas(canvas), arena(canvas)
{

}

void DemoGround::show()
{
	(void)getwch();
}
