#pragma once
#include "DescriptorHeap.h"
#include "GraphicsCore.h"
#include "TextureManager.h"

class TextureStorage : public TSingleton<TextureStorage>
{
public:
	void Load()
	{
		// ¼ÓÔØÍ¼Æ¬
		TextureHeap.Create(L"Scene Texture Descriptors", D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 4096);

		D3D12_CPU_DESCRIPTOR_HANDLE handle = TextureHeap.Alloc(2);

		TextureReferences.resize(6);
		TextureReferences[0] = TextureManager::LoadDDSFromFile(L"../Assets/Textures/WoodCrate02.dds");
		TextureReferences[1] = TextureManager::LoadDDSFromFile(L"../Assets/Textures/SkyBox.dds");
		TextureReferences[2] = TextureManager::LoadDDSFromFile(L"../Assets/Textures/outBrdf.dds");
		TextureReferences[3] = TextureManager::LoadDDSFromFile(L"../Assets/Textures/outDiffuseHDR.dds");
		TextureReferences[4] = TextureManager::LoadDDSFromFile(L"../Assets/Textures/outEnvHDR.dds");
		TextureReferences[5] = TextureManager::LoadDDSFromFile(L"../Assets/Textures/outSpecularHDR.dds");

		uint32_t DestCount = 2;
		uint32_t SourceCounts[] = { 1, 1 };

		D3D12_CPU_DESCRIPTOR_HANDLE SourceTextures[] =
		{
			TextureReferences[0].GetSRV(),
			TextureReferences[4].GetSRV()
		};

		Graphics::g_Device->CopyDescriptors(
			1,
			&handle,
			&DestCount,
			DestCount,
			SourceTextures,
			SourceCounts,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	// Textures
	std::vector<TextureRef> TextureReferences;

	// TextureµÄÃèÊö·û¶Ñ
	DescriptorHeap TextureHeap;
};
