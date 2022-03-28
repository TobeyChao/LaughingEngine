#pragma once
#include "PixelBuffer.h"

class DepthBuffer : public PixelBuffer
{
public:
	DepthBuffer()
	{
		m_hDSV.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
		m_hSRV.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	}
	void Create(const std::wstring Name, uint32_t Width, uint32_t Height, DXGI_FORMAT Format);

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetDSV() const { return m_hDSV; }
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV() const { return m_hSRV; }

private:
	void CreateDerivedViews(ID3D12Device* Device, DXGI_FORMAT Format);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE m_hDSV;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hSRV;
};