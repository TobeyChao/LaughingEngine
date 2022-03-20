#pragma once
#include "GpuResource.h"

class PixelBuffer : public GpuResource
{
public:
	PixelBuffer()
		:
		m_Width(0),
		m_Height(0),
		m_ArraySize(0),
		m_Format(DXGI_FORMAT_UNKNOWN)
	{
	}
	uint32_t GetWidth(void) const { return m_Width; }
	uint32_t GetHeight(void) const { return m_Height; }
	uint32_t GetDepth(void) const { return m_ArraySize; }
	const DXGI_FORMAT& GetFormat(void) const { return m_Format; }

protected:
	void AssociateWithResource(
		ID3D12Device* device,
		const std::wstring& name,
		ID3D12Resource* resource,
		D3D12_RESOURCE_STATES currentState);

	D3D12_RESOURCE_DESC DescribeTex2D(
		uint32_t width,
		uint32_t height,
		uint32_t depthOrArraySize,
		uint32_t numMips,
		DXGI_FORMAT format,
		UINT flags);

	void CreateTextureResource(
		ID3D12Device* device,
		const std::wstring& name,
		const D3D12_RESOURCE_DESC& resourceDesc,
		D3D12_CLEAR_VALUE clearValue);

protected:
	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_ArraySize;
	DXGI_FORMAT m_Format;
};