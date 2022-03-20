#include "ColorBuffer.h"
#include "GraphicsCore.h"

void ColorBuffer::CreateFromSwapChain(const std::wstring& name, ID3D12Resource* baseResource)
{
	AssociateWithResource(Graphics::g_Device, name, baseResource, D3D12_RESOURCE_STATE_PRESENT);

	m_RTVHandle = Graphics::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	Graphics::g_Device->CreateRenderTargetView(baseResource, nullptr, m_RTVHandle);
}

void ColorBuffer::Create(const std::wstring& name, uint32_t width, uint32_t height, uint32_t numMips, DXGI_FORMAT format)
{
	numMips = numMips == 0 ? ComputeNumMips(width, height) : numMips;

	D3D12_RESOURCE_FLAGS flag = CombineResourceFlags();
	D3D12_RESOURCE_DESC desc = DescribeTex2D(
		width,
		height,
		1,
		numMips,
		format,
		flag
	);

	desc.SampleDesc.Count = m_FragmentCount;
	desc.SampleDesc.Quality = 0;

	D3D12_CLEAR_VALUE clear = {};
	clear.Format = format;
	memcpy(clear.Color, m_ClearColor, sizeof(float) * 4);

	CreateTextureResource(Graphics::g_Device, name, desc, clear);
	CreateDerivedViews(Graphics::g_Device, format, 1, numMips);
}

void ColorBuffer::CreateDerivedViews(ID3D12Device* device, DXGI_FORMAT format, uint32_t arraySize, uint32_t numMips)
{
	m_NumMipMaps = numMips - 1;

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
	SRVDesc.Texture2D.MipLevels = numMips;
	SRVDesc.Texture2D.MostDetailedMip = 0;

	ID3D12Resource* res = m_pResource.Get();

	if (m_SRVHandle.ptr == -1)
	{
		m_SRVHandle = Graphics::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	if (m_RTVHandle.ptr == -1)
	{
		m_RTVHandle = Graphics::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	device->CreateRenderTargetView(
		res,
		&RTVDesc,
		m_RTVHandle);

	device->CreateShaderResourceView(
		res,
		&SRVDesc,
		m_SRVHandle);

	for (uint32_t i = 0; i < numMips; i++)
	{
		if (m_UAVHandle[i].ptr == -1)
		{
			m_UAVHandle[i] = Graphics::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}

		device->CreateUnorderedAccessView(
			res,
			nullptr,
			&UAVDesc,
			m_UAVHandle[i]);
	}
}
