#pragma once
#include "DescriptorHeap.h"
#include "GraphicsCore.h"
#include "TextureManager.h"

class TextureStorage : public TSingleton<TextureStorage>
{
public:
	void Load()
	{
		// ????ͼƬ
		TextureHeap.Create(L"Scene Texture Descriptors", D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 4096);

		DescriptorHandle handle = TextureHeap.Alloc(10);

		TextureReferences.resize(10);
		TextureReferences[0] = TextureManager::LoadFromFile(L"Assets\\Textures\\WoodCrate02.dds");
		TextureReferences[1] = TextureManager::LoadFromFile(L"Assets\\Textures\\SkyBox.dds");
		TextureReferences[2] = TextureManager::LoadFromFile(L"Assets\\Textures\\outBrdf.dds");
		TextureReferences[3] = TextureManager::LoadFromFile(L"Assets\\Textures\\outDiffuseHDR.dds");
		TextureReferences[4] = TextureManager::LoadFromFile(L"Assets\\Textures\\outEnvHDR.dds");
		TextureReferences[5] = TextureManager::LoadFromFile(L"Assets\\Textures\\outSpecularHDR.dds");
		TextureReferences[6] = TextureManager::LoadFromFile(L"Assets\\Meshes\\textures\\cerberus_A.png");
		TextureReferences[7] = TextureManager::LoadFromFile(L"Assets\\Meshes\\textures\\cerberus_M.png");
		TextureReferences[8] = TextureManager::LoadFromFile(L"Assets\\Meshes\\textures\\cerberus_N.png");
		TextureReferences[9] = TextureManager::LoadFromFile(L"Assets\\Meshes\\textures\\cerberus_R.png");

		uint32_t DestCount = 10;
		uint32_t SourceCounts[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

		D3D12_CPU_DESCRIPTOR_HANDLE SourceTextures[] =
		{
			TextureReferences[0].GetSRV(),
			TextureReferences[1].GetSRV(),
			TextureReferences[2].GetSRV(),
			TextureReferences[3].GetSRV(),
			TextureReferences[4].GetSRV(),
			TextureReferences[5].GetSRV(),
			TextureReferences[6].GetSRV(),
			TextureReferences[7].GetSRV(),
			TextureReferences[8].GetSRV(),
			TextureReferences[9].GetSRV()
		};

		TextureReferences[0].SetIndexInHeap(0);
		TextureReferences[1].SetIndexInHeap(1);
		TextureReferences[2].SetIndexInHeap(2);
		TextureReferences[3].SetIndexInHeap(3);
		TextureReferences[4].SetIndexInHeap(4);
		TextureReferences[5].SetIndexInHeap(5);
		TextureReferences[6].SetIndexInHeap(6);
		TextureReferences[7].SetIndexInHeap(7);
		TextureReferences[8].SetIndexInHeap(8);
		TextureReferences[9].SetIndexInHeap(9);

		Graphics::g_Device->CopyDescriptors(
			1,
			&handle,
			&DestCount,
			DestCount,
			SourceTextures,
			SourceCounts,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void Shutdown()
	{
		TextureReferences.clear();
		TextureHeap.Destroy();
	}

	// Textures
	std::vector<TextureRef> TextureReferences;

	// Texture??????????
	DescriptorHeap TextureHeap;
};
