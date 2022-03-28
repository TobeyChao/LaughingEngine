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
	{}

	uint32_t GetWidth(void) const { return m_Width; }
	uint32_t GetHeight(void) const { return m_Height; }
	uint32_t GetDepth(void) const { return m_ArraySize; }
	const DXGI_FORMAT& GetFormat(void) const { return m_Format; }

protected:
	void AssociateWithResource(
		ID3D12Device* Device,
		const std::wstring& Name,
		ID3D12Resource* Res,
		D3D12_RESOURCE_STATES CurState);

	D3D12_RESOURCE_DESC DescribeTex2D(
		uint32_t Width,
		uint32_t Height,
		uint32_t DepthOrArratSize,
		uint32_t NumMips,
		DXGI_FORMAT Format,
		UINT Flags);

	void CreateTextureResource(
		ID3D12Device* Device,
		const std::wstring& Name,
		const D3D12_RESOURCE_DESC& Desc,
		D3D12_CLEAR_VALUE Clear);

	static DXGI_FORMAT GetBaseFormat(DXGI_FORMAT Format);
	static DXGI_FORMAT GetUAVFormat(DXGI_FORMAT Format);
	static DXGI_FORMAT GetDSVFormat(DXGI_FORMAT Format);
	static DXGI_FORMAT GetDepthFormat(DXGI_FORMAT Format);
	static DXGI_FORMAT GetStencilFormat(DXGI_FORMAT Format);
	static size_t BytesPerPixel(DXGI_FORMAT Format);

protected:
	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_ArraySize;
	DXGI_FORMAT m_Format;
};