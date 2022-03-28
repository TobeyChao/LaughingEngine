#pragma once

#include "PCH.h"

#include <vector>
#include <queue>

/// <summary>
/// ÃèÊö·û¾ä±ú
/// </summary>
class DescriptorHandle
{
public:
	DescriptorHandle()
	{
		m_CpuHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
		m_GpuHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	}

	DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
		:
		m_CpuHandle(cpuHandle),
		m_GpuHandle(gpuHandle)
	{
	}

	DescriptorHandle operator+ (int OffsetScaledByDescriptorSize)
	{
		DescriptorHandle ret = *this;
		ret += OffsetScaledByDescriptorSize;
		return ret;
	}

	DescriptorHandle operator+ (INT OffsetScaledByDescriptorSize) const
	{
		DescriptorHandle ret = *this;
		ret += OffsetScaledByDescriptorSize;
		return ret;
	}

	void operator+=(int offsetScaledByDescriptorSize)
	{
		if (m_CpuHandle.ptr != -1)
		{
			m_CpuHandle.ptr += offsetScaledByDescriptorSize;
		}
		if (m_GpuHandle.ptr != -1)
		{
			m_GpuHandle.ptr += offsetScaledByDescriptorSize;
		}
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE* operator&() const { return &m_CpuHandle; }

	operator D3D12_CPU_DESCRIPTOR_HANDLE() const { return m_CpuHandle; }

	operator D3D12_GPU_DESCRIPTOR_HANDLE() const { return m_GpuHandle; }

private:
	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE m_GpuHandle;
};

/// <summary>
/// ÃèÊö·û¶Ñ
/// </summary>
class DescriptorHeap
{
public:
	DescriptorHeap()
		:
		m_Desc(),
		m_DescriptorSize(0),
		m_NumFreeDescriptors(0)
	{}

	~DescriptorHeap()
	{
		Destroy();
	}

	void Create(const std::wstring& name, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t count);

	void Destroy()
	{
		m_Heap.Reset();
	}

	bool HasAvailableSpace(uint32_t count) const { return m_NumFreeDescriptors >= count; }

	DescriptorHandle Alloc(uint32_t count = 1);

	DescriptorHandle operator[] (uint32_t arrayIdx) const
	{
		return m_FirstHandle + arrayIdx * m_DescriptorSize;
	}

	ID3D12DescriptorHeap* GetDescriptorHeapPointer() const
	{
		return m_Heap.Get();
	}

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
	D3D12_DESCRIPTOR_HEAP_DESC m_Desc;
	// ÃèÊö·û´óÐ¡
	uint32_t m_DescriptorSize;
	uint32_t m_NumFreeDescriptors;
	DescriptorHandle m_FirstHandle;
	DescriptorHandle m_NextFreeHandle;
};
