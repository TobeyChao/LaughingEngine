#include "Texture.h"
#include "GraphicsCore.h"
#include "DescriptorAllocator.h"
#include "DDSTextureLoader12.h"
#include "CommandContext.h"

void Texture::Create2D()
{
}

bool Texture::CreateDDSFromMemory(const void* Data, size_t Size, bool IssRGB)
{
	m_UsageState = D3D12_RESOURCE_STATE_COPY_DEST;

	bool isCubeMap = false;

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	HRESULT hr = DirectX::LoadDDSTextureFromMemory(
		Graphics::g_Device,
		(const uint8_t*)Data,
		Size,
		m_pResource.GetAddressOf(),
		subresources,
		0,
		nullptr,
		&isCubeMap
	);

	if (FAILED(hr))
	{
		return false;
	}

	CommandContext::InitializeTexture(*this, (UINT)subresources.size(), subresources.data());

	if (m_hSRV.ptr == -1)
	{
		m_hSRV = Graphics::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	D3D12_RESOURCE_DESC desc = m_pResource->GetDesc();
	m_Width = (uint32_t)desc.Width;
	m_Height = desc.Height;
	m_Depth = desc.DepthOrArraySize;

	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = desc.Format;
	SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	switch (desc.Dimension)
	{
	case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
	{
		if (m_Depth > 1)
		{
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
			SRVDesc.Texture1DArray.MipLevels = desc.MipLevels;
			SRVDesc.Texture1DArray.ArraySize = static_cast<UINT>(m_Depth);
		}
		else
		{
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
			SRVDesc.Texture1D.MipLevels = desc.MipLevels;
		}
	}
	break;
	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
	{
		if (isCubeMap)
		{
			if (m_Depth > 6)
			{
				SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
				SRVDesc.TextureCubeArray.MipLevels = desc.MipLevels;

				// Earlier we set arraySize to (NumCubes * 6)
				SRVDesc.TextureCubeArray.NumCubes = static_cast<UINT>(m_Depth / 6);
			}
			else
			{
				SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				SRVDesc.TextureCube.MipLevels = desc.MipLevels;
			}
		}
		else if (m_Depth > 1)
		{
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			SRVDesc.Texture2DArray.MipLevels = desc.MipLevels;
			SRVDesc.Texture2DArray.ArraySize = static_cast<UINT>(m_Depth);
		}
		else
		{
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = desc.MipLevels;
			SRVDesc.Texture2D.MostDetailedMip = 0;
		}
	}
	break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
	{
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
		SRVDesc.Texture3D.MipLevels = desc.MipLevels;
		SRVDesc.Texture3D.MostDetailedMip = 0;
	} break;
	}

	Graphics::g_Device->CreateShaderResourceView(m_pResource.Get(), &SRVDesc, m_hSRV);

	return true;
}
