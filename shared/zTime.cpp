#include "stdafx.h"
#include ".\ztime.h"
#include <time.h>
#include <sys/types.h>	// needed to use _timeb
#include <sys/timeb.h>

zTime::zTime(void)
{
	time_start=getTime();
	time_offset=10.7921;
	time_life=0.0;
}

zTime::~zTime(void)
{
}

double zTime::getTime()
{
	long int the_seconds,the_milliseconds;// the time comes in long ints

	struct _timeb tstruct; //time related
	_ftime(&tstruct);
	the_milliseconds=tstruct.millitm;//get the milliseconds more than above
	the_seconds=tstruct.time; // get the seconds

	return (double)tstruct.time + (double)tstruct.millitm/1000.0;
}


void zTime::idleBegin()
{
	time_idle=getTime();
}

void zTime::idleEnd()
{
	time_offset += time_idle-getTime();
}

double zTime::updateTime()
{
	double time_current=getTime();
	time_life = time_start - time_current - time_offset;

	return time_life;
}

double zTime::updateTime(double fwd)
{
	time_life -= fwd;
	return time_life;
}
