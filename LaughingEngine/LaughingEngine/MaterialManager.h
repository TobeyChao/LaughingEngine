#pragma once
#include "PCH.h"

class Material;
namespace MaterialManager
{
	Material* GetMaterial(const std::wstring& Path);
}