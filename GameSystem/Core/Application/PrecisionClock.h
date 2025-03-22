#pragma once

namespace GameSystem {
	class PrecisionClock
	{
	public:
		static void Init();
		static double GetDeltaTime();
	private:
		static double secPerTick;
		static LARGE_INTEGER freq;
		static LARGE_INTEGER lastTick;
	};
}