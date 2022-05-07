#pragma once
#include "TSingleton.h"
#include <unordered_map>
#include <memory>

class ShaderProgram;
class ShaderManager : public TSingleton<ShaderManager>
{
public:
	std::shared_ptr<ShaderProgram> GetShader(
		const std::wstring& Path,
		const std::wstring& VertexPath,
		const std::wstring& PixelPath);

private:
	std::shared_ptr<ShaderProgram> FindOrLoadShader(const std::wstring& Path,
		const std::wstring& VertexPath,
		const std::wstring& PixelPath);

private:
	std::unordered_map<std::wstring, std::shared_ptr<ShaderProgram>> m_Shaders;
};
