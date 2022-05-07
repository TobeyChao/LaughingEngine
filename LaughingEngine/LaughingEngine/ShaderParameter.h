#pragma once
#include <string>

enum class ParameterType
{
	Scalar,
	Vector,
	Matrix,
	Struct,

	Texture,
	CBuffer,
	TBuffer,
	Count,
};

class ShaderParameter
{
public:
	virtual ~ShaderParameter() = default;
	[[nodiscard]] inline virtual ParameterType GetType() const = 0;
	unsigned int Index = 0;
	unsigned int Space = 0;
	unsigned int Count = 0;
	std::string Name;
};
