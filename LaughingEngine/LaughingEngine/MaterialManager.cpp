#include "PCH.h"
#include "MaterialManager.h"
#include "ShaderManager.h"
#include "Material.h"
#include <json.hpp>

using namespace std;
using namespace nlohmann;
using namespace MaterialSerialization;

namespace MaterialManager
{
	static std::unordered_map<std::wstring, std::shared_ptr<Material>> s_Materials;

	Material* FindOrLoadMaterial(const std::wstring& Path)
	{
		if (!s_Materials.contains(Path))
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
			material->MaterialObject = j.get<MaterialObject>();
			auto& [name] = material->MaterialObject.m_ShaderObject;
			const std::wstring path = Utility::UTF8ToWideString(name);
			material->ShaderProgram = ShaderManager::GetInstance().GetShader(path);

			// ¼ÓÔØÍ¼Æ¬
			for (auto& [ID, Res] : material->MaterialObject.m_Properties.m_Textures)
			{
				const TextureRef texRef = TextureManager::LoadFromFile(Utility::UTF8ToWideString(Res.GUID));
				material->SetTexture(ID, texRef);
			}

			s_Materials[Path] = std::move(material);
		}

		return s_Materials[Path].get();
	}

	Material* GetMaterial(const std::wstring& Path)
	{
		return FindOrLoadMaterial(Path);
	}

	void Shutdown()
	{
		s_Materials.clear();
	}
}