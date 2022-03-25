#pragma once

#include <cstdint>

namespace Graphics
{
	extern unsigned int g_DisplayWidth;
	extern unsigned int g_DisplayHeight;
	extern bool g_bEnableHDROutput;

	uint64_t GetFrameCount();
	float GetFrameTime();
	float GetFrameRate();
}

namespace Display
{
	void Initialize();
	void Shutdown();
	void Resize(uint32_t width, uint32_t height);
	void Update();
	void Present();
}