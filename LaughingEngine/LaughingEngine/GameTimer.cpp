#include "PCH.h"
#include "GameTimer.h"

double GameTimer::m_SecondsPerCount = 0.0f;
double GameTimer::m_DeltaTime = 0.0f;

int64_t GameTimer::m_BaseTime = 0;
int64_t GameTimer::m_PausedTime = 0;
int64_t GameTimer::m_StopTime = 0;
int64_t GameTimer::m_PrevTime = 0;
int64_t GameTimer::m_CurTime = 0;

bool GameTimer::m_Stopped = false;

void GameTimer::Initialize()
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	m_SecondsPerCount = 1.0 / static_cast<double>(frequency.QuadPart);
}

double GameTimer::TotalTime()
{
	if (m_Stopped)
	{
		return ((m_StopTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount;
	}
	else
	{
		return ((m_CurTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount;
	}
}

double GameTimer::DeltaTime()
{
	return m_DeltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_BaseTime = currTime;
	m_PrevTime = currTime;
	m_StopTime = 0;
	m_Stopped = false;
}

void GameTimer::Start()
{
	__int64 startTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (m_Stopped)
	{
		m_PausedTime += (startTime - m_StopTime);

		m_PrevTime = startTime;
		m_StopTime = 0;
		m_Stopped = false;
	}
}

void GameTimer::Stop()
{
	if (!m_Stopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_StopTime = currTime;
		m_Stopped = true;
	}
}

void GameTimer::Tick()
{
	if (m_Stopped)
	{
		m_DeltaTime = 0.0;
		return;
	}

	__int64 curTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&curTime);
	m_CurTime = curTime;

	// Time difference between this frame and the previous.
	m_DeltaTime = (m_CurTime - m_PrevTime) * m_SecondsPerCount;

	// Prepare for next frame.
	m_PrevTime = m_CurTime;

	// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
	// processor goes into a power save mode or we get shuffled to another
	// processor, then mDeltaTime can be negative.
	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}
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