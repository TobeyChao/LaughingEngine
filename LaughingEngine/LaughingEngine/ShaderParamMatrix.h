#pragma once
#include "ShaderParamVariable.h"

class ShaderParamMatrix : public ShaderParamVariable
{
public:
	ParameterType GetType() const override
	{
		return ParameterType::Matrix;
	}
};
