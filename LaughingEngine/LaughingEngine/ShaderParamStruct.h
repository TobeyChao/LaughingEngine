#pragma once
#include "ShaderParamVariable.h"

class ShaderParamStruct : public ShaderParamVariable
{
public:
	ParameterType GetType() const override
	{
		return ParameterType::Struct;
	}
};
