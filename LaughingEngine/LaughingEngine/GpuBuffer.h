#pragma once
#include "GpuResource.h"

class CommandContext;
class UploadBuffer;

class GpuBuffer : public GpuResource
{
public:
	GpuBuffer()
		:
		m_BufferSize(0),
		m_ElementCount(0),
		m_ElementSize(0)
	{
		m_hUAV.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
		m_hSRV.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
		m_ResourceFlags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	~GpuBuffer() override
	{
	}

	void Create(const std::wstring& name, uint32_t ElementSize, uint32_t ElementCount, const void* Data);
	void Create(const std::wstring& name, uint32_t ElementSize, uint32_t ElementCount, const UploadBuffer& Buffer, uint32_t SrcOffset);

	// 创建常量静态缓冲区
	// 当作根常量缓冲区可以不需要描述符
	// 放进描述符表则需要
	D3D12_CPU_DESCRIPTOR_HANDLE CreateConstanceBuffer(uint32_t Offset, uint32_t SizeInBytes) const;

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetUAV() const { return m_hUAV; }
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV() const { return m_hSRV; }

	size_t BufferSize() const { return m_BufferSize; }
	uint32_t ElementCount() const { return m_ElementCount; }
	uint32_t ElementSize() const { return m_ElementSize; }

protected:
	virtual void CreateDerivedViews() = 0;

protected:
	D3D12_CPU_DESCRIPTOR_HANDLE m_hUAV{};
	D3D12_CPU_DESCRIPTOR_HANDLE m_hSRV{};

	size_t m_BufferSize;
	uint32_t m_ElementCount;
	uint32_t m_ElementSize;

	D3D12_RESOURCE_FLAGS m_ResourceFlags;
};

class IndexBuffer : public GpuBuffer
{
public:
	// 索引IndexView
	// 不需要放到描述符堆
	D3D12_INDEX_BUFFER_VIEW IndexBufferView(size_t Offset, uint32_t SizeInBytes, bool Is32Bit) const;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView(size_t StartIndex) const
	{
		size_t Offset = StartIndex * m_ElementSize;
		return IndexBufferView(Offset, (uint32_t)(m_BufferSize - Offset), m_ElementSize == 4);
	}

protected:
	virtual void CreateDerivedViews()
	{
		// DO NOTHING.
	}
};

class VertexBuffer : public GpuBuffer
{
public:
	// 顶点VertexView
	// 不需要放到描述符堆上
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView(size_t Offset, uint32_t SizeInBytes, uint32_t StrideInBytes) const;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView(size_t StartIndex) const
	{
		size_t Offset = StartIndex * m_ElementSize;
		return VertexBufferView(Offset, (uint32_t)(m_BufferSize - Offset), m_ElementSize);
	}

protected:
	virtual void CreateDerivedViews()
	{
		// DO NOTHING.
	}
};

class ByteAddressBuffer : public GpuBuffer
{
public:
	virtual void CreateDerivedViews() override;

};

class IndirectArgsBuffer : public ByteAddressBuffer
{
public:
};

class StructuredBuffer : public GpuBuffer
{
public:
	virtual void Destroy() override
	{
		m_CounterBuffer.Destroy();
		GpuBuffer::Destroy();
	}

	virtual void CreateDerivedViews() override;

	ByteAddressBuffer& GetCounterBuffer()
	{
		return m_CounterBuffer;
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCounterSRV(CommandContext& Context);
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCounterUAV(CommandContext& Context);

private:
	ByteAddressBuffer m_CounterBuffer;
};

class TypedBuffer : public GpuBuffer
{
public:
	TypedBuffer(DXGI_FORMAT Format)
		:
		m_DataFormat(Format)
	{}

	virtual void CreateDerivedViews() override;

protected:
	DXGI_FORMAT m_DataFormat;
};