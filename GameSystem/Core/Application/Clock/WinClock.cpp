#include "pch.h"
#include "WinClock.h"

WinClock::WinClock()
{
	QueryPerformanceFrequency(&frequency);
	sec_per_tick = 1.0 / static_cast<double>(frequency.QuadPart);
	QueryPerformanceCounter(&last_tick);
}

double WinClock::GetDeltaTime()
{
	LARGE_INTEGER curr_tick;
	QueryPerformanceCounter(&curr_tick);
	double delta_time = (curr_tick.QuadPart - last_tick.QuadPart) * sec_per_tick;
	last_tick = curr_tick;
	return delta_time;
}