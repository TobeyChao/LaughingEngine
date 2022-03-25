#pragma once
#include "PixelBuffer.h"

class DepthBuffer : public PixelBuffer
{
public:
	DepthBuffer()
	{
		m_DSVHandle.ptr = -1;
		m_SRVHandle.ptr = -1;
	}
	void Create(const std::wstring Name, uint32_t Width, uint32_t Height, DXGI_FORMAT Format);

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetDSV() const { return m_DSVHandle; }
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV() const { return m_SRVHandle; }

private:
	void CreateDerivedViews(ID3D12Device* Device, DXGI_FORMAT Format);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE m_DSVHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_SRVHandle;
};