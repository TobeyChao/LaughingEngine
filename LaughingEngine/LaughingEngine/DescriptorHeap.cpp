#include "DescriptorHeap.h"

#include "GraphicsCore.h"

std::mutex DescriptorAllocator::sm_AllocationMutex;
std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> DescriptorAllocator::sm_DescriptorHeapPool;

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::Alloc(uint32_t count)
{
	// 如果当前没有存储的堆或者当前的堆不够了
	if (m_CurrentHeap == nullptr || m_RemainingFreeHandles < count)
	{
		// 申请一个新的堆
		m_CurrentHeap = RequestNewHeap(m_Type);
		m_CurrentHandle = m_CurrentHeap->GetCPUDescriptorHandleForHeapStart();
		m_RemainingFreeHandles = sm_NumDescriptorsPerHeap;

		if (m_DescriptorSize == 0)
		{
			m_DescriptorSize = Graphics::g_Device->GetDescriptorHandleIncrementSize(m_Type);
		}
	}

	// 当前的handle就是未使用的第一个handle
	D3D12_CPU_DESCRIPTOR_HANDLE ret = m_CurrentHandle;
	// 将当前handle指针后移
	m_CurrentHandle.ptr += m_DescriptorSize * count;
	// 更新剩余的handle数量
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

	// 创建描述符堆
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

void DescriptorHeap::Create(const std::wstring& name, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t count)
{
	m_Desc.Type = heapType;
	m_Desc.NodeMask = 1;
	m_Desc.NumDescriptors = count;
	m_Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	Graphics::g_Device->CreateDescriptorHeap(&m_Desc, IID_PPV_ARGS(m_Heap.GetAddressOf()));

#if defined(_DEBUG) || defined(DEBUG)
	m_Heap->SetName(name.c_str());
#endif // defined(_DEBUG) || defined(DEBUG)

	m_DescriptorSize = Graphics::g_Device->GetDescriptorHandleIncrementSize(heapType);

	m_NumFreeDescriptors = count;
	m_FirstHandle = DescriptorHandle(
		m_Heap->GetCPUDescriptorHandleForHeapStart(),
		m_Heap->GetGPUDescriptorHandleForHeapStart());
	m_NextFreeHandle = m_FirstHandle;
}

DescriptorHandle DescriptorHeap::Alloc(uint32_t count)
{
	assert(HasAvailableSpace(count));
	DescriptorHandle ret = m_NextFreeHandle;
	m_NextFreeHandle += count * m_DescriptorSize;
	m_NumFreeDescriptors -= count;
	return ret;
}