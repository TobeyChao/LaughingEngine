#include "UploadBuffer.h"
#include "GraphicsCore.h"

using namespace Graphics;

void UploadBuffer::Create(const std::wstring& name, size_t BufferSize)
{
	Destroy();

	m_BufferSize = BufferSize;

	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(m_BufferSize);
	m_UsageState = D3D12_RESOURCE_STATE_GENERIC_READ;
	CD3DX12_HEAP_PROPERTIES HeapProps(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(g_Device->CreateCommittedResource(
		&HeapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		m_UsageState,
		nullptr,
		IID_PPV_ARGS(m_pResource.GetAddressOf())));

	m_GpuVirtualAddress = m_pResource->GetGPUVirtualAddress();

#if defined(DEBUG) || defined(_DEBUG)
	m_pResource->SetName(name.c_str());
#endif // defined(DEBUG) || defined(_DEBUG)
}

void* UploadBuffer::Map()
{
	void* Memory = nullptr;
	CD3DX12_RANGE range(0, m_BufferSize);
	m_pResource->Map(0, &range, &Memory);
	return Memory;
}

void UploadBuffer::Unmap(size_t begin, size_t end)
{
	CD3DX12_RANGE range(begin, std::min(end, m_BufferSize));
	m_pResource->Unmap(0, &range);
}