#include "PixelBuffer.h"

void PixelBuffer::AssociateWithResource(
	ID3D12Device* device,
	const std::wstring& name,
	ID3D12Resource* resource,
	D3D12_RESOURCE_STATES currentState)
{
	D3D12_RESOURCE_DESC resourceDesc = resource->GetDesc();

	m_pResource.Attach(resource);
	m_UsageState = currentState;
	m_Width = (uint32_t)resourceDesc.Width;
	m_Height = resourceDesc.Height;
	m_ArraySize = resourceDesc.DepthOrArraySize;
	m_Format = resourceDesc.Format;
#if defined(DEBUG) || defined(_DEBUG)
	m_pResource->SetName(name.c_str());
#endif //defined(DEBUG) || defined(_DEBUG)
}

D3D12_RESOURCE_DESC PixelBuffer::DescribeTex2D(
	uint32_t width,
	uint32_t height,
	uint32_t depthOrArraySize,
	uint32_t numMips,
	DXGI_FORMAT format,
	UINT flags)
{
	m_Width = width;
	m_Height = height;
	m_ArraySize = depthOrArraySize;
	m_Format = format;

	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
		m_Format,
		m_Width,
		m_Height,
		m_ArraySize,
		numMips,
		1,
		0,
		(D3D12_RESOURCE_FLAGS)flags,
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		0
	);

	return desc;
}

void PixelBuffer::CreateTextureResource(
	ID3D12Device* device,
	const std::wstring& name,
	const D3D12_RESOURCE_DESC& resourceDesc,
	D3D12_CLEAR_VALUE clearValue)
{
	Destroy();

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
	device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&clearValue,
		IID_PPV_ARGS(m_pResource.GetAddressOf()));
	m_UsageState = D3D12_RESOURCE_STATE_COMMON;
	m_GpuVirtualAddress = 0;

#if defined(DEBUG) || defined(_DEBUG)
	m_pResource->SetName(name.c_str());
#endif defined(DEBUG) || defined(_DEBUG)
}