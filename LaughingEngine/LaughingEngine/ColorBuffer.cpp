#include "PCH.h"
#include "ColorBuffer.h"
#include "GraphicsCore.h"
#include "DescriptorAllocator.h"

void ColorBuffer::CreateFromSwapChain(const std::wstring& name, ID3D12Resource* baseResource)
{
	AssociateWithResource(Graphics::g_Device, name, baseResource, D3D12_RESOURCE_STATE_PRESENT);

	m_hRTV = Graphics::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	Graphics::g_Device->CreateRenderTargetView(baseResource, nullptr, m_hRTV);
}

void ColorBuffer::Create(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t NumMips, DXGI_FORMAT Format)
{
	NumMips = NumMips == 0 ? ComputeNumMips(Width, Height) : NumMips;

	D3D12_RESOURCE_FLAGS flag = CombineResourceFlags();

	D3D12_RESOURCE_DESC desc = DescribeTex2D(Width, Height, 1, NumMips, Format, (UINT)flag);

	D3D12_CLEAR_VALUE clear;
	ZeroMemory(&clear, sizeof(clear));
	clear.Format = Format;
	memcpy(clear.Color, m_ClearColor, sizeof(float) * 4);

	CreateTextureResource(Graphics::g_Device, Name, desc, clear);
	CreateDerivedViews(Graphics::g_Device, Format, 1, NumMips);
}

void ColorBuffer::CreateDerivedViews(ID3D12Device* device, DXGI_FORMAT format, uint32_t ArraySize, uint32_t NumMips)
{
	m_NumMipMaps = NumMips - 1;

	D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = {};
	D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};

	RTVDesc.Format = format;
	RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;

	UAVDesc.Format = format;
	UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	UAVDesc.Texture2D.MipSlice = 0;

	SRVDesc.Format = format;
	SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = NumMips;
	SRVDesc.Texture2D.MostDetailedMip = 0;

	ID3D12Resource* res = m_pResource.Get();

	if (m_hSRV.ptr == -1)
	{
		m_hSRV = Graphics::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	if (m_hRTV.ptr == -1)
	{
		m_hRTV = Graphics::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	device->CreateRenderTargetView(res, &RTVDesc, m_hRTV);

	device->CreateShaderResourceView(res, &SRVDesc, m_hSRV);

	for (uint32_t i = 0; i < NumMips; i++)
	{
		if (m_hUAV[i].ptr == -1)
		{
			m_hUAV[i] = Graphics::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}

		device->CreateUnorderedAccessView(res, nullptr, &UAVDesc, m_hUAV[i]);
	}
}
