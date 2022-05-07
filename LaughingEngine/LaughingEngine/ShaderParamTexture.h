#pragma once
#include "ShaderParameter.h"

class ShaderParamTexture : public ShaderParameter
{
public:
	ParameterType GetType() const override
	{
		return ParameterType::Texture;
	}

	// �ߴ�
	D3D_SRV_DIMENSION Dimension;
};
