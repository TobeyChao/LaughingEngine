#include "PCH.h"
#include "MaterialManager.h"
#include "ShaderManager.h"
#include "Material.h"
#include <fstream>
#include <json.hpp>

using namespace std;
using namespace nlohmann;
using namespace MaterialSerialization;

std::shared_ptr<Material> MaterialManager::GetMaterial(const std::wstring& Path)
{
	return FindOrLoadMaterial(Path);
}

std::shared_ptr<Material> MaterialManager::FindOrLoadMaterial(const std::wstring& Path)
{
	if (!m_Materials.contains(Path))
	{
		std::shared_ptr<Material> material = std::make_shared<Material>();

		ifstream file;
		file.open(Path, ios::in);
		json j;
		if (file.is_open())
		{
			file >> j;
		}
		file.close();
		material->m_MaterialObject = j.get<MaterialObject>();
		auto& [name, shader] = material->m_MaterialObject.m_ShaderObject;
		const std::wstring path = Utility::UTF8ToWideString(name);
		const std::wstring vertexShaderPath = Utility::UTF8ToWideString(shader[(int)ShaderType::Vertex].GUID);
		const std::wstring pixelShaderPath = Utility::UTF8ToWideString(shader[(int)ShaderType::Pixel].GUID);
		material->m_ShaderProgram = ShaderManager::GetInstance().GetShader(path, vertexShaderPath, pixelShaderPath);
		m_Materials[Path] = std::move(material);
	}

	return m_Materials[Path];
}