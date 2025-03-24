#pragma once

namespace GameSystem {
	class PrecisionClock
	{
	public:
		PrecisionClock();
		~PrecisionClock();
		double GetDeltaTime();
	private:
		double secPerTick;
		LARGE_INTEGER freq;
		LARGE_INTEGER lastTick;
	};
}