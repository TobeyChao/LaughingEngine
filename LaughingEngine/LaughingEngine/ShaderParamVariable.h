#pragma once
#include <d3dcommon.h>
#include <string>

#include "ShaderParameter.h"

class ShaderParamCBuffer;
class ShaderParamVariable : public ShaderParameter
{
public:
	ShaderParamCBuffer* BindBuffer{};

	// ��CBuffer�е�ƫ��
	unsigned int StartOffset = 0;
	unsigned int Size = 0;

	D3D_SHADER_VARIABLE_CLASS Class{};
	D3D_SHADER_VARIABLE_TYPE Type{};
	unsigned int Rows = 0;
	unsigned int Columns = 0;
	unsigned int Elements = 0;
	unsigned int Members = 0;
	// �ڽṹ���е�Offset
	unsigned int Offset{};
	std::string Name;
};
