#pragma once
#ifndef Z_TIMER_H
#define Z_TIMER_H

#include <ctime>

class zTimer
{
public:
	zTimer(void);
	~zTimer(void);
	void start();
	int end();
private:
	clock_t counter;
};
#endif