#pragma once
#include <DirectXMath.h>
#include <DirectXColors.h>
#include "PixelBuffer.h"

class ColorBuffer : public PixelBuffer
{
public:
	ColorBuffer()
		:
		m_ClearColor(DirectX::Colors::SkyBlue),
		m_NumMipMaps(0),
		m_FragmentCount(1),
		m_SampleCount(1)
	{
		m_hRTV.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
		m_hSRV.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
		for (int i = 0; i < _countof(m_hUAV); ++i)
		{
			m_hUAV[i].ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
		}
	}

	/// <summary>
	/// 从交换链创建Buffer
	/// </summary>
	/// <param name="name"></param>
	/// <param name="baseResource"></param>
	void CreateFromSwapChain(const std::wstring& Name, ID3D12Resource* BaseResource);
	void Create(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t NumMips, DXGI_FORMAT Format);

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV() const { return m_hSRV; }
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTV() const { return m_hRTV; }
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetUAV() const { return m_hUAV[0]; }

	const DirectX::XMVECTORF32& GetClearColor() const { return m_ClearColor; }

	/// <summary>
	/// 设置和资源创建的时候不同的颜色会导致Warning
	/// </summary>
	/// <param name="color"></param>
	void SetClearColor(const DirectX::XMVECTORF32& color) { m_ClearColor = color; }

private:
	D3D12_RESOURCE_FLAGS CombineResourceFlags() const
	{
		D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE;

		// 不启用MSAA GPU可以写入
		if (Flags == D3D12_RESOURCE_FLAG_NONE && m_FragmentCount == 1)
		{
			Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		return D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | Flags;
	}

	static inline uint32_t ComputeNumMips(uint32_t Width, uint32_t Height)
	{
		uint32_t highBit = 0;
		_BitScanReverse((unsigned long*)&highBit, Width | Height);
		return highBit + 1;
	}

	void CreateDerivedViews(ID3D12Device* Device, DXGI_FORMAT Format, uint32_t ArraySize, uint32_t NumMips = 1);

private:
	DirectX::XMVECTORF32 m_ClearColor;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hSRV;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hRTV;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hUAV[12];
	uint32_t m_NumMipMaps;
	uint32_t m_FragmentCount;
	uint32_t m_SampleCount;
};