#pragma once

#include "PCH.h"

#include <mutex>
#include <vector>
#include <queue>
#include <string>

/// <summary>
/// 描述符申请器
/// ShaderVisible = false
/// 一种描述符申请器只能申请特定种类的描述符
/// D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
/// D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
/// D3D12_DESCRIPTOR_HEAP_TYPE_RTV
/// D3D12_DESCRIPTOR_HEAP_TYPE_DSV
/// </summary>
class DescriptorAllocator
{
public:
	/// <summary>
	/// 构造函数
	/// </summary>
	/// <param name="type">描述符的类型</param>
	DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type)
		:
		m_Type(type),
		m_CurrentHeap(nullptr),
		m_DescriptorSize(0),
		m_RemainingFreeHandles(0)
	{
		m_CurrentHandle.ptr = -1;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Alloc(uint32_t count);

	static void DestroyAll();

protected:
	static ID3D12DescriptorHeap* RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);

protected:
	static const uint32_t sm_NumDescriptorsPerHeap = 256;
	static std::mutex sm_AllocationMutex;
	static std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> sm_DescriptorHeapPool;

	D3D12_DESCRIPTOR_HEAP_TYPE m_Type;
	// 申请的描述符所存放的堆
	ID3D12DescriptorHeap* m_CurrentHeap;
	// 描述符句柄
	D3D12_CPU_DESCRIPTOR_HANDLE m_CurrentHandle;
	// 描述符大小
	uint32_t m_DescriptorSize;
	// 当前堆还能使用的描述符
	uint32_t m_RemainingFreeHandles;
};

/// <summary>
/// 描述符句柄
/// </summary>
class DescriptorHandle
{
public:
	DescriptorHandle()
	{
		m_CpuHandle.ptr = -1;
		m_GpuHandle.ptr = -1;
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
/// 描述符堆
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
	// 描述符大小
	uint32_t m_DescriptorSize;
	uint32_t m_NumFreeDescriptors;
	DescriptorHandle m_FirstHandle;
	DescriptorHandle m_NextFreeHandle;
};
