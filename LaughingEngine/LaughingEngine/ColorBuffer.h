#pragma once
#include <DirectXMath.h>
#include <DirectXColors.h>
#include "PixelBuffer.h"

class ColorBuffer : public PixelBuffer
{
public:
	ColorBuffer()
		:
		m_ClearColor(DirectX::Colors::Black),
		m_NumMipMaps(0),
		m_FragmentCount(1),
		m_SampleCount(1)
	{
		m_RTVHandle.ptr = -1;
		m_SRVHandle.ptr = -1;
		for (int i = 0; i < _countof(m_UAVHandle); ++i)
			m_UAVHandle[i].ptr = -1;
	}

	/// <summary>
	/// 从交换链创建Buffer
	/// </summary>
	/// <param name="name"></param>
	/// <param name="baseResource"></param>
	void CreateFromSwapChain(const std::wstring& name, ID3D12Resource* baseResource);
	void Create(const std::wstring& name, uint32_t width, uint32_t height, uint32_t numMips, DXGI_FORMAT format);

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV() const { return m_SRVHandle; }
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTV() const { return m_RTVHandle; }
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetUAV() const { return m_UAVHandle[0]; }

	DirectX::XMVECTORF32 GetClearColor() const { return m_ClearColor; }

	/// <summary>
	/// 设置和资源创建的时候不同的颜色会导致Warning
	/// </summary>
	/// <param name="color"></param>
	void SetClearColor(const DirectX::XMVECTORF32& color) { m_ClearColor = color; }

private:
	D3D12_RESOURCE_FLAGS CombineResourceFlags() const
	{
		D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE;

		if (Flags == D3D12_RESOURCE_FLAG_NONE && m_FragmentCount == 1)
			Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		return D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | Flags;
	}

	static inline uint32_t ComputeNumMips(uint32_t width, uint32_t height)
	{
		uint32_t highBit;
		_BitScanReverse((unsigned long*)&highBit, width | height);
		return highBit + 1;
	}

	void CreateDerivedViews(ID3D12Device* device,
		DXGI_FORMAT format,
		uint32_t arraySize,
		uint32_t numMips = 1);

private:
	DirectX::XMVECTORF32 m_ClearColor;
	D3D12_CPU_DESCRIPTOR_HANDLE m_SRVHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_UAVHandle[12];
	uint32_t m_NumMipMaps;
	uint32_t m_FragmentCount;
	uint32_t m_SampleCount;
};