#pragma once
#include "ShaderParamVariable.h"

class ShaderParamVector : public ShaderParamVariable
{
public:
	ParameterType GetType() const override
	{
		return ParameterType::Vector;
	}
};
