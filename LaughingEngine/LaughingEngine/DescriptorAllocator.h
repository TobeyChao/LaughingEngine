#pragma once
#include "PCH.h"
#include <mutex>

/// <summary>
/// ������������
/// non shader-visible descriptor heap.
/// һ��������������ֻ�������ض������������
/// D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
/// D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
/// D3D12_DESCRIPTOR_HEAP_TYPE_RTV
/// D3D12_DESCRIPTOR_HEAP_TYPE_DSV
/// </summary>
class DescriptorAllocator
{
public:
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="type">������������</param>
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
	// ���������������ŵĶ�
	ID3D12DescriptorHeap* m_CurrentHeap;
	// ���������
	D3D12_CPU_DESCRIPTOR_HANDLE m_CurrentHandle;
	// ��������С
	uint32_t m_DescriptorSize;
	// ��ǰ�ѻ���ʹ�õ�������
	uint32_t m_RemainingFreeHandles;
};

namespace Graphics
{
	extern DescriptorAllocator g_DescriptorAllocator[];
	inline D3D12_CPU_DESCRIPTOR_HANDLE AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t Count = 1u)
	{
		return g_DescriptorAllocator[Type].Alloc(Count);
	}
}