#include "PCH.h"
#include "ShaderManager.h"
#include "ShaderProgram.h"

using namespace std;

std::shared_ptr<ShaderProgram> ShaderManager::GetShader(
	const std::wstring& Path,
	const std::wstring& VertexPath,
	const std::wstring& PixelPath)
{
	return FindOrLoadShader(Path, VertexPath, PixelPath);
}

std::shared_ptr<ShaderProgram> ShaderManager::FindOrLoadShader(
	const std::wstring& Name,
	const std::wstring& VertexPath,
	const std::wstring& PixelPath)
{
	if (!m_Shaders.contains(Name))
	{
		std::shared_ptr<ShaderProgram> shader = std::make_shared<ShaderProgram>();
		const auto vertex = Utility::LoadFileSync(VertexPath);
		const auto pixel = Utility::LoadFileSync(PixelPath);
		shader->LoadFromMemory(vertex, pixel);
		m_Shaders[Name] = std::move(shader);
	}

	return m_Shaders[Name];
}
