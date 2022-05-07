#pragma once
#include "PCH.h"
#include "ShaderParameter.h"
#include "ShaderType.h"

class ShaderProgram
{
	friend class ShaderManager;
public:
	ShaderProgram() = default;

	void LoadFromMemory(Utility::ByteArray, Utility::ByteArray);
	bool CheckType(const std::string& ID, ParameterType Type);
	ShaderParameter* GetParam(const std::string& ID);

private:
	void ParseFromMemory(ShaderType type, Utility::ByteArray bytes);
	void AddParameter(const std::string& Name, ShaderParameter* parameter);
	void AddInputElement(const std::string& SemanticName, DXGI_FORMAT Format, UINT AlignedByteOffset);

	void OutputBoundResources(UINT i, const D3D12_SHADER_INPUT_BIND_DESC& bind_desc);
	void OutputVariable(UINT j, ID3D12ShaderReflectionVariable* variable);
	void OutputType(ID3D12ShaderReflectionType* type);

private:
	std::unordered_map<std::string, ShaderParameter*> m_Parameters;
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;
};