#include "PCH.h"
#include "DescriptorHeap.h"
#include "GraphicsCore.h"

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