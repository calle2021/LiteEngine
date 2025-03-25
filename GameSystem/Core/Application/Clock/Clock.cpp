#include "pch.h"
#include "Clock.h"
#include "Core/Application/Clock/WinClock.h"

std::unique_ptr<Clock> Clock::Create()
{
	return std::make_unique<WinClock>();
}