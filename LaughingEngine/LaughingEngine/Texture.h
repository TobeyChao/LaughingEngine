#pragma once

#include "GpuResource.h"

class Texture : public GpuResource
{
public:
	Texture()
		:
		m_Width(0),
		m_Height(0),
		m_Depth(0)
	{
		m_hSRV.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	}

	Texture(D3D12_CPU_DESCRIPTOR_HANDLE Handle)
		:
		m_Width(0),
		m_Height(0),
		m_Depth(0),
		m_hSRV(Handle)
	{}

	void Create2D();

	bool CreateDDSFromMemory(const void* Data, size_t Size, bool IssRGB);

	virtual void Destroy() override
	{
		GpuResource::Destroy();
		m_hSRV.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV() const
	{
		return m_hSRV;
	}

	uint32_t GetWidth() const { return m_Width; };
	uint32_t GetHeight() const { return m_Height; };
	uint32_t GetDepth() const { return m_Depth; };

protected:
	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_Depth;

	D3D12_CPU_DESCRIPTOR_HANDLE m_hSRV;
};