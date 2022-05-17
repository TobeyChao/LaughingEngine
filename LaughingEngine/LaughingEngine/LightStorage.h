#pragma once
#include "PCH.h"
#include <DirectXMath.h>

using namespace DirectX;

class LightStorage : public TSingleton<LightStorage>
{
public:
	void Shutdown()
	{

	}

	// 直射光方向控制变量
	float Theta = 0.0f;
};
