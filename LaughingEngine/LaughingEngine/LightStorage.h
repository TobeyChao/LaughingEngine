#pragma once
#include "PCH.h"
#include <DirectXMath.h>

using namespace DirectX;

class LightStorage : public TSingleton<LightStorage>
{
public:
	// ֱ��ⷽ����Ʊ���
	float Theta = 0.0f;
};
