#include "DepthBuffer.h"
#include "GraphicsCore.h"

void DepthBuffer::Create(const std::wstring Name, uint32_t Width, uint32_t Height, DXGI_FORMAT Format)
{
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
		Format, Width, Height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	D3D12_CLEAR_VALUE clear;
	ZeroMemory(&clear, sizeof(clear));
	clear.Format = Format;

	CreateTextureResource(Graphics::g_Device, Name, desc, clear);
	CreateDerivedViews(Graphics::g_Device, Format);
}

void DepthBuffer::CreateDerivedViews(ID3D12Device* Device, DXGI_FORMAT Format)
{
	ID3D12Resource* res = m_pResource.Get();

	D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	ZeroMemory(&DSVDesc, sizeof(DSVDesc));
	DSVDesc.Format = GetDSVFormat(Format);
	DSVDesc.Texture2D.MipSlice = 0;
	DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Flags = D3D12_DSV_FLAG_NONE;

	if (m_DSVHandle.ptr == -1)
	{
		m_DSVHandle = Graphics::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}
	Device->CreateDepthStencilView(res, &DSVDesc, m_DSVHandle);

	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRVDesc.Format = GetDepthFormat(Format);
	SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;
	SRVDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	SRVDesc.Texture2D.PlaneSlice = 0;

	if (m_SRVHandle.ptr == -1)
	{
		m_SRVHandle = Graphics::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	Device->CreateShaderResourceView(res, &SRVDesc, m_SRVHandle);
}