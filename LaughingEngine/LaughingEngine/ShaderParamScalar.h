#pragma once
#include "ShaderParamVariable.h"

class ShaderParamScalar : public ShaderParamVariable
{
public:
	ParameterType GetType() const override
	{
		return ParameterType::Scalar;
	}
};
