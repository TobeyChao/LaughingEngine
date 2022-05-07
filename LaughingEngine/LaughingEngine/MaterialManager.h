#pragma once
#include "PCH.h"

class Material;
class MaterialManager final : public TSingleton<MaterialManager>
{
public:
	std::shared_ptr<Material> GetMaterial(const std::wstring& Path);

private:
	std::shared_ptr<Material> FindOrLoadMaterial(const std::wstring& Path);

private:
	std::unordered_map<std::wstring, std::shared_ptr<Material>> m_Materials;
};