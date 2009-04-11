#pragma once

class zTime
{
public:
	zTime(void);
	~zTime(void);

	double getTime();
	void idleBegin();
	void idleEnd();
	double elapseTime();

	double updateTime();// real-time
	double updateTime(double fwd);// play every frame

private:
	double time_start, time_idle, time_offset, time_life;
};
