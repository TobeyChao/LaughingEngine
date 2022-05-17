#pragma once
#include "PCH.h"
#include "TSingleton.h"
#include "ShaderType.h"

class ShaderProgram;
class ShaderManager : public TSingleton<ShaderManager>
{
public:
	void RegisterShader(const std::wstring& Name, const std::wstring& VertexPath, const std::wstring& PixelPath, bool LoadImmediately = false);
	std::shared_ptr<ShaderProgram> GetShader(const std::wstring& Name);
	void Shutdown();

private:
	std::shared_ptr<ShaderProgram> FindOrLoadShader(const std::wstring& Name);

private:
	std::unordered_map<std::wstring, std::shared_ptr<ShaderProgram>> m_Shaders;
	std::unordered_map<std::wstring, std::wstring[(size_t)ShaderType::Count]> m_ShaderMapper;
};
