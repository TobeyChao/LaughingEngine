#include "DescriptorAllocator.h"
#include "GraphicsCore.h"

std::mutex DescriptorAllocator::sm_AllocationMutex;
std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> DescriptorAllocator::sm_DescriptorHeapPool;

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::Alloc(uint32_t count)
{
	// �����ǰû�д洢�Ķѻ��ߵ�ǰ�ĶѲ�����
	if (m_CurrentHeap == nullptr || m_RemainingFreeHandles < count)
	{
		// ����һ���µĶ�
		m_CurrentHeap = RequestNewHeap(m_Type);
		m_CurrentHandle = m_CurrentHeap->GetCPUDescriptorHandleForHeapStart();
		m_RemainingFreeHandles = sm_NumDescriptorsPerHeap;

		if (m_DescriptorSize == 0)
		{
			m_DescriptorSize = Graphics::g_Device->GetDescriptorHandleIncrementSize(m_Type);
		}
	}

	// ��ǰ��handle����δʹ�õĵ�һ��handle
	D3D12_CPU_DESCRIPTOR_HANDLE ret = m_CurrentHandle;
	// ����ǰhandleָ�����
	m_CurrentHandle.ptr += (SIZE_T)m_DescriptorSize * count;
	// ����ʣ���handle����
	m_RemainingFreeHandles -= count;
	return ret;
}

void DescriptorAllocator::DestroyAll()
{
	sm_DescriptorHeapPool.clear();
}

ID3D12DescriptorHeap* DescriptorAllocator::RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	std::lock_guard<std::mutex> lockGuarad(sm_AllocationMutex);

	// ������������
	D3D12_DESCRIPTOR_HEAP_DESC desc;
	desc.Type = type;
	desc.NumDescriptors = sm_NumDescriptorsPerHeap;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 1;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
	Graphics::g_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(heap.GetAddressOf()));
	sm_DescriptorHeapPool.emplace_back(heap);
	return heap.Get();
}
namespace Graphics
{
	DescriptorAllocator g_DescriptorAllocator[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] =
	{
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV
	};
}