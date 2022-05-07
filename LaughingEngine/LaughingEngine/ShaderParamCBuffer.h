#pragma once
#include "ShaderParameter.h"
#include <cstdint>

class ShaderParamCBuffer : public ShaderParameter
{
public:
	[[nodiscard]] ParameterType GetType() const override
	{
		return ParameterType::CBuffer;
	}

	UINT Variables;
	UINT Size;
};
