#pragma once

#include <cstdint>

class GameTimer
{
public:
	static void Initialize();

	static double TotalTime(); // in seconds
	static double DeltaTime(); // in seconds

	static void Reset(); // Call before message loop.
	static void Start(); // Call when unpaused.
	static void Stop();  // Call when paused.
	static void Tick();  // Call every frame.

	static int64_t GetCurrentTick();
	static double DeltaTimeMillisecs(int64_t tick1, int64_t tick2);
	static double DeltaTimeSeconds(int64_t tick1, int64_t tick2);

private:
	static double m_SecondsPerCount;
	static double m_DeltaTime;

	static int64_t m_BaseTime;
	static int64_t m_PausedTime;
	static int64_t m_StopTime;
	static int64_t m_PrevTime;
	static int64_t m_CurTime;

	static bool m_Stopped;
};