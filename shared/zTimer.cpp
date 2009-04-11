#include "zTimer.h"

zTimer::zTimer(void)
{
}

zTimer::~zTimer(void)
{
}

void zTimer::start()
{
	clock_t tick = std::clock();
	counter = tick;
}

int zTimer::end()
{
	clock_t tick = std::clock();
	return int(tick-counter);
}
