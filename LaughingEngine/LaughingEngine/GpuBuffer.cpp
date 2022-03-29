#include "GpuBuffer.h"
#include "GraphicsCore.h"
#include "DescriptorAllocator.h"
#include "CommandContext.h"

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

	if (data)
	{
		CommandContext::InitializeBuffer(*this, data, m_BufferSize);
	}

#if defined(DEBUG) || defined(_DEBUG)
	m_pResource->SetName(name.c_str());
#endif // defined(DEBUG) || defined(_DEBUG)

	CreateDerivedViews();
}

void GpuBuffer::Create(const std::wstring& name, uint32_t ElementSize, uint32_t ElementCount, const UploadBuffer& Buffer, uint32_t SrcOffset)
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

	CommandContext::InitializeBuffer(*this, Buffer, SrcOffset);

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

D3D12_INDEX_BUFFER_VIEW IndexBuffer::IndexBufferView(size_t Offset, uint32_t SizeInBytes, bool Is32Bit) const
{
	D3D12_INDEX_BUFFER_VIEW view;
	ZeroMemory(&view, sizeof(view));
	view.BufferLocation = m_GpuVirtualAddress + Offset;
	view.SizeInBytes = SizeInBytes;
	view.Format = Is32Bit ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
	return view;
}

D3D12_VERTEX_BUFFER_VIEW VertexBuffer::VertexBufferView(size_t Offset, uint32_t SizeInBytes, uint32_t StrideInBytes) const
{
	D3D12_VERTEX_BUFFER_VIEW view;
	ZeroMemory(&view, sizeof(view));
	view.BufferLocation = m_GpuVirtualAddress + Offset;
	view.SizeInBytes = SizeInBytes;
	view.StrideInBytes = StrideInBytes;
	return view;
}

void ByteAddressBuffer::CreateDerivedViews()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = (UINT)m_BufferSize / 4;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

	if (m_hSRV.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
	{
		m_hSRV = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	g_Device->CreateShaderResourceView(m_pResource.Get(), &srvDesc, m_hSRV);

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	uavDesc.Buffer.NumElements = (UINT)m_BufferSize / 4;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

	if (m_hUAV.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
	{
		m_hUAV = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	g_Device->CreateUnorderedAccessView(m_pResource.Get(), nullptr, &uavDesc, m_hUAV);

}

void StructuredBuffer::CreateDerivedViews()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = m_ElementCount;
	srvDesc.Buffer.StructureByteStride = m_ElementSize;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	if (m_hSRV.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
	{
		m_hSRV = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	g_Device->CreateShaderResourceView(m_pResource.Get(), &srvDesc, m_hSRV);

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.NumElements = m_ElementCount;
	uavDesc.Buffer.StructureByteStride = m_ElementSize;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	if (m_hUAV.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
	{
		m_hUAV = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	g_Device->CreateUnorderedAccessView(m_pResource.Get(), nullptr, &uavDesc, m_hUAV);
}

void TypedBuffer::CreateDerivedViews(void)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Format = m_DataFormat;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = m_ElementCount;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	if (m_hSRV.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
	{
		m_hSRV = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	g_Device->CreateShaderResourceView(m_pResource.Get(), &srvDesc, m_hSRV);

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Format = m_DataFormat;
	uavDesc.Buffer.NumElements = m_ElementCount;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	if (m_hUAV.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
	{
		m_hUAV = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	g_Device->CreateUnorderedAccessView(m_pResource.Get(), nullptr, &uavDesc, m_hUAV);
}