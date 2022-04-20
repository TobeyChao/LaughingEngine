#pragma once
#include "PCH.h"
#include "GameTimer.h"
#include <DirectXMath.h>

using namespace DirectX;

class LightManager : public TSingleton<LightManager>
{
public:
	void Update()
	{
		Theta += (float)GameTimer::DeltaTime();
		if (Theta > XM_2PI)
		{
			Theta = 0.0f;
		}
	}

	// ֱ��ⷽ����Ʊ���
	float Theta = 0.0f;
};
