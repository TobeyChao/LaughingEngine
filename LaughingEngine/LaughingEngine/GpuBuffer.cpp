#include "GpuBuffer.h"
#include "GraphicsCore.h"
#include "DescriptorAllocator.h"

using namespace Graphics;

void GpuBuffer::Create(const std::wstring& name, uint32_t ElementSize, uint32_t ElementCount, const void* data)
{
	Destroy();

	m_ElementCount = ElementCount;
	m_ElementSize = ElementSize;
	m_BufferSize = ElementCount * ElementSize;

	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(m_BufferSize, m_ResourceFlags);
	m_UsageState = D3D12_RESOURCE_STATE_COMMON;
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

	CreateDerivedViews();
}

void GpuBuffer::Create(const std::wstring& name, uint32_t ElementSize, uint32_t ElementCount, const UploadBuffer& buffer)
{
	Destroy();

	m_ElementCount = ElementCount;
	m_ElementSize = ElementSize;
	m_BufferSize = ElementCount * ElementSize;

	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(m_BufferSize, m_ResourceFlags);
	m_UsageState = D3D12_RESOURCE_STATE_COMMON;
	CD3DX12_HEAP_PROPERTIES HeapProps(D3D12_HEAP_TYPE_DEFAULT);
	g_Device->CreateCommittedResource(
		&HeapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		m_UsageState,
		nullptr,
		IID_PPV_ARGS(m_pResource.GetAddressOf()));

	m_GpuVirtualAddress = m_pResource->GetGPUVirtualAddress();

#if defined(DEBUG) || defined(_DEBUG)
	m_pResource->SetName(name.c_str());
#endif // defined(DEBUG) || defined(_DEBUG)

	CreateDerivedViews();
}

D3D12_CPU_DESCRIPTOR_HANDLE GpuBuffer::CreateConstanceBuffer(uint32_t Offset, uint32_t SizeInBytes) const
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferLocation = m_GpuVirtualAddress + Offset;
	desc.SizeInBytes = (UINT)Math::AlignUp(SizeInBytes, 256);
	D3D12_CPU_DESCRIPTOR_HANDLE handle = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	g_Device->CreateConstantBufferView(&desc, handle);
	return handle;
}

D3D12_VERTEX_BUFFER_VIEW GpuBuffer::VertexBufferView(size_t Offset, uint32_t SizeInBytes, uint32_t StrideInBytes) const
{
	D3D12_VERTEX_BUFFER_VIEW view;
	ZeroMemory(&view, sizeof(view));
	view.BufferLocation = m_GpuVirtualAddress + Offset;
	view.SizeInBytes = SizeInBytes;
	view.StrideInBytes = StrideInBytes;
	return view;
}

D3D12_INDEX_BUFFER_VIEW GpuBuffer::IndexBufferView(size_t Offset, uint32_t SizeInBytes, bool Is32Bit) const
{
	D3D12_INDEX_BUFFER_VIEW view;
	ZeroMemory(&view, sizeof(view));
	view.BufferLocation = m_GpuVirtualAddress + Offset;
	view.SizeInBytes = SizeInBytes;
	view.Format = Is32Bit ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
	return view;
}

void ByteAddressBuffer::CreateDerivedViews()
{
}