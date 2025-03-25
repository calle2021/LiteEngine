#pragma once

class Clock
{
public:
	virtual ~Clock() = default;
	virtual double GetDeltaTime() = 0;
	static std::unique_ptr<Clock> Create();
};
