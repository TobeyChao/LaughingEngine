#include "PCH.h"
#include "GameTimer.h"

double GameTimer::m_SecondsPerCount = 0.0f;

void GameTimer::Initialize()
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	m_SecondsPerCount = 1.0 / static_cast<double>(frequency.QuadPart);
}

int64_t GameTimer::GetCurrentTick()
{
	LARGE_INTEGER currentTick;
	QueryPerformanceCounter(&currentTick);
	return static_cast<int64_t>(currentTick.QuadPart);
}

double GameTimer::DeltaTimeMillisecs(int64_t tick1, int64_t tick2)
{
	return (tick2 - tick1) * m_SecondsPerCount * 1000.0;
}

double GameTimer::DeltaTimeSeconds(int64_t tick1, int64_t tick2)
{
	return (tick2 - tick1) * m_SecondsPerCount;
}