#include "PCH.h"
#include "ShaderManager.h"
#include "ShaderProgram.h"

using namespace std;

void ShaderManager::RegisterShader(const std::wstring& Name, const std::wstring& VertexPath, const std::wstring& PixelPath, bool LoadImmediately)
{
	if (m_Shaders.contains(Name))
	{
		return;
	}
	m_ShaderMapper[Name][(size_t)ShaderType::Vertex] = VertexPath;
	m_ShaderMapper[Name][(size_t)ShaderType::Pixel] = PixelPath;
	if (LoadImmediately)
	{
		GetShader(Name);
	}
}

std::shared_ptr<ShaderProgram> ShaderManager::GetShader(const std::wstring& Name)
{
	return FindOrLoadShader(Name);
}

void ShaderManager::Shutdown()
{
	m_ShaderMapper.clear();
	m_Shaders.clear();
}

std::shared_ptr<ShaderProgram> ShaderManager::FindOrLoadShader(const std::wstring& Name)
{
	if (!m_Shaders.contains(Name))
	{
		std::shared_ptr<ShaderProgram> shader = std::make_shared<ShaderProgram>();
		const wstring& VertexPath = m_ShaderMapper[Name][(size_t)ShaderType::Vertex];
		const wstring& PixelPath = m_ShaderMapper[Name][(size_t)ShaderType::Pixel];
		m_ShaderMapper[Name][(size_t)ShaderType::Pixel] = PixelPath;
		const auto vertex = Utility::LoadFileSync(VertexPath);
		const auto pixel = Utility::LoadFileSync(PixelPath);
		shader->LoadFromMemory(vertex, pixel);
		m_Shaders[Name] = std::move(shader);
	}

	return m_Shaders[Name];
}
