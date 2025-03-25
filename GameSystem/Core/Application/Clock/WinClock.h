#pragma once
#include "Core/Application/Clock/Clock.h"

class WinClock : public Clock
{
public:
	WinClock();
	double GetDeltaTime() override;
private:
	double sec_per_tick;
	LARGE_INTEGER frequency;
	LARGE_INTEGER last_tick;
};
