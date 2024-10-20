#include "pixlpch.h"
#include "PrecisionClock.h"

namespace PIXL {
	double PrecisionClock::secPerTick;
	LARGE_INTEGER PrecisionClock::freq;
	LARGE_INTEGER PrecisionClock::lastTick;

	void PrecisionClock::Init()
	{
		QueryPerformanceFrequency(&freq);
		secPerTick = 1.0 / static_cast<double>(freq.QuadPart);
		QueryPerformanceCounter(&lastTick);
	}
	double PrecisionClock::GetDeltaTime()
	{
		LARGE_INTEGER currTick;
		QueryPerformanceCounter(&currTick);
		double deltaTime = (currTick.QuadPart - lastTick.QuadPart) * secPerTick;
		lastTick = currTick;
		return deltaTime;
	}
}