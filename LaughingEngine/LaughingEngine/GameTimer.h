#pragma once

#include <cstdint>

class GameTimer
{
public:
	static void Initialize();

	static int64_t GetCurrentTick();
	static double DeltaTimeMillisecs(int64_t tick1, int64_t tick2);
	static double DeltaTimeSeconds(int64_t tick1, int64_t tick2);

private:
	static double m_SecondsPerCount;
};