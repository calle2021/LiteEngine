#include "pch.h"
#include "PrecisionClock.h"

namespace GameSystem {
	PrecisionClock::PrecisionClock()
	{
		QueryPerformanceFrequency(&freq);
		secPerTick = 1.0 / static_cast<double>(freq.QuadPart);
		QueryPerformanceCounter(&lastTick);
	}

	PrecisionClock::~PrecisionClock()
	{
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