#pragma once
#include "PCH.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "GpuBuffer.h"
#include "MemoryAllocator.h"
#include "DynamicDescriptorHeap.h"

class GraphicsContext;
class ComputeContext;

class CommandContext
{
	friend class ContextManager;

public:
	CommandContext(D3D12_COMMAND_LIST_TYPE Type);
	~CommandContext();

	void Initialize();
	void Reset();

	static CommandContext& Begin(const std::wstring& id = L"");

	uint64_t Flush(bool WaitForCompletion = false);
	uint64_t Finish(bool WaitForCompletion = false);

	// 获取上下文
	GraphicsContext& GetGraphicsContext()
	{
		return reinterpret_cast<GraphicsContext&>(*this);
	}
	ComputeContext& GetComputeContext()
	{
		return reinterpret_cast<ComputeContext&>(*this);
	}
	ID3D12GraphicsCommandList* GetCommandList()
	{
		return m_CommandList;
	}

	MemoryHandle ReserveUploadMemory(size_t SizeInBytes)
	{
		return m_CpuMemoryAllocator.Allocate(SizeInBytes);
	}

	static void InitializeTexture(GpuResource& Dest, UINT NumSubresources, D3D12_SUBRESOURCE_DATA SubData[]);
	static void InitializeBuffer(GpuBuffer& Dest, const void* Data, size_t NumBytes, size_t DestOffset = 0);
	static void InitializeBuffer(GpuBuffer& Dest, const UploadBuffer& Src, size_t SrcOffset, size_t NumBytes = -1, size_t DestOffset = 0);

	// 资源转换
	void TransitionResource(GpuResource& res, D3D12_RESOURCE_STATES targetState, bool immediate = false);
	void FlushResourceBarrier();

	// 设置描述符堆
	void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12DescriptorHeap* heap);
	void SetDescriptorHeaps(UINT heapCount, D3D12_DESCRIPTOR_HEAP_TYPE type[], ID3D12DescriptorHeap* heap[]);
	void BindDescriptorHeaps();

	// 设置PSO
	void SetPipelineState(const PipelineState& pso);

	void SetID(const std::wstring& id) { m_ID = id; }

protected:
	ID3D12CommandAllocator* m_Allocator;
	ID3D12GraphicsCommandList* m_CommandList;

	ID3D12RootSignature* m_GraphicsRootSignature;
	ID3D12RootSignature* m_ComputeRootSignature;
	ID3D12PipelineState* m_PipelineState;

	DynamicDescriptorHeap m_DynamicViewDescriptorHeap;		// HEAP_TYPE_CBV_SRV_UAV
	DynamicDescriptorHeap m_DynamicSamplerDescriptorHeap;	// HEAP_TYPE_SAMPLER

	D3D12_RESOURCE_BARRIER m_ResourceBarrier[16];
	UINT m_NumBarriers;

	ID3D12DescriptorHeap* m_DescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

	MemoryAllocator m_CpuMemoryAllocator;
	MemoryAllocator m_GpuMemoryAllocator;

	std::wstring m_ID;
	D3D12_COMMAND_LIST_TYPE m_Type;
};

class GraphicsContext : public CommandContext
{
public:
	static GraphicsContext& Begin(const std::wstring& id = L"");

	void ClearColor(ColorBuffer& Target, const D3D12_RECT* Rect);
	void ClearDepth(DepthBuffer& Target);

	void SetRootSignature(const RootSignature& rootSignature);

	void SetRenderTargets(UINT NumRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE RTV[]);
	void SetRenderTargets(UINT NumRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE RTV[], D3D12_CPU_DESCRIPTOR_HANDLE DSV);
	void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv) { SetRenderTargets(1, &rtv); }
	void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv) { SetRenderTargets(1, &rtv, dsv); }
	void SetDepthStencilTarget(D3D12_CPU_DESCRIPTOR_HANDLE dsv) { SetRenderTargets(0, nullptr, dsv); }

	void SetViewport(FLOAT TopLeftX, FLOAT TopLeftY, FLOAT Width, FLOAT Height, FLOAT MinDepth = 0.0f, FLOAT MaxDepth = 1.0f);
	void SetScissorRect(LONG left, LONG top, LONG right, LONG bottom);
	void SetViewportAndScissorRect(UINT x, UINT y, UINT w, UINT h);
	void SetViewport(const D3D12_VIEWPORT& Viewport);
	void SetScissorRect(const D3D12_RECT& Rect);
	void SetViewportAndScissorRect(const D3D12_VIEWPORT& Viewport, const D3D12_RECT& Rect);
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology);

	void SetConstantBuffer(UINT RootIndex, D3D12_GPU_VIRTUAL_ADDRESS CBV);
	void SetDynamicConstantBufferView(UINT RootIndex, size_t BufferSize, const void* BufferData);
	void SetBufferSRV(UINT RootIndex, const GpuBuffer& SRV, UINT64 Offset = 0);
	void SetBufferUAV(UINT RootIndex, const GpuBuffer& UAV, UINT64 Offset = 0);
	void SetDescriptorTable(UINT RootIndex, D3D12_GPU_DESCRIPTOR_HANDLE FirstHandle);

	void SetDynamicDescriptor(UINT RootIndex, UINT Offset, D3D12_CPU_DESCRIPTOR_HANDLE Handle);
	void SetDynamicDescriptors(UINT RootIndex, UINT Offset, UINT Count, const D3D12_CPU_DESCRIPTOR_HANDLE Handles[]);
	void SetDynamicSampler(UINT RootIndex, UINT Offset, D3D12_CPU_DESCRIPTOR_HANDLE Handle);
	void SetDynamicSamplers(UINT RootIndex, UINT Offset, UINT Count, const D3D12_CPU_DESCRIPTOR_HANDLE Handles[]);

	void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& IBView);
	void SetVertexBuffer(UINT Slot, const D3D12_VERTEX_BUFFER_VIEW& VBView);

	void DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation = 0, UINT StartInstanceLocation = 0);
	void DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation);
};

inline void CommandContext::SetPipelineState(const PipelineState& pso)
{
	m_CommandList->SetPipelineState(pso.GetPSO());
}

inline void GraphicsContext::SetRootSignature(const RootSignature& rootSignature)
{
	if (rootSignature.GetRootSignature() == m_GraphicsRootSignature)
		return;
	m_GraphicsRootSignature = rootSignature.GetRootSignature();
	m_CommandList->SetGraphicsRootSignature(rootSignature.GetRootSignature());
}

inline void GraphicsContext::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology)
{
	m_CommandList->IASetPrimitiveTopology(PrimitiveTopology);
}

inline void GraphicsContext::SetConstantBuffer(UINT RootIndex, D3D12_GPU_VIRTUAL_ADDRESS CBV)
{
	m_CommandList->SetGraphicsRootConstantBufferView(RootIndex, CBV);
}

inline void GraphicsContext::SetDynamicConstantBufferView(UINT RootIndex, size_t BufferSize, const void* BufferData)
{
	MemoryHandle cb = m_CpuMemoryAllocator.Allocate(BufferSize);
	memcpy(cb.CpuAddress, BufferData, BufferSize);
	m_CommandList->SetGraphicsRootConstantBufferView(RootIndex, cb.GpuAddress);
}

inline void GraphicsContext::SetBufferSRV(UINT RootIndex, const GpuBuffer& SRV, UINT64 Offset)
{
	m_CommandList->SetGraphicsRootConstantBufferView(RootIndex, SRV.GetGpuAddress() + Offset);
}

inline void GraphicsContext::SetBufferUAV(UINT RootIndex, const GpuBuffer& UAV, UINT64 Offset)
{
	m_CommandList->SetGraphicsRootUnorderedAccessView(RootIndex, UAV.GetGpuAddress() + Offset);
}

inline void GraphicsContext::SetDescriptorTable(UINT RootIndex, D3D12_GPU_DESCRIPTOR_HANDLE FirstHandle)
{
	m_CommandList->SetGraphicsRootDescriptorTable(RootIndex, FirstHandle);
}

inline void GraphicsContext::SetDynamicDescriptor(UINT RootIndex, UINT Offset, D3D12_CPU_DESCRIPTOR_HANDLE Handle)
{
	SetDynamicDescriptors(RootIndex, Offset, 1, &Handle);
}

inline void GraphicsContext::SetDynamicDescriptors(UINT RootIndex, UINT Offset, UINT Count, const D3D12_CPU_DESCRIPTOR_HANDLE Handles[])
{
	m_DynamicViewDescriptorHeap.SetGraphicsDescriptorHandles(RootIndex, Offset, Count, Handles);
}

inline void GraphicsContext::SetDynamicSampler(UINT RootIndex, UINT Offset, D3D12_CPU_DESCRIPTOR_HANDLE Handle)
{
	SetDynamicSamplers(RootIndex, Offset, 1, &Handle);
}

inline void GraphicsContext::SetDynamicSamplers(UINT RootIndex, UINT Offset, UINT Count, const D3D12_CPU_DESCRIPTOR_HANDLE Handles[])
{
	m_DynamicSamplerDescriptorHeap.SetGraphicsDescriptorHandles(RootIndex, Offset, Count, Handles);
}

inline void GraphicsContext::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& IBView)
{
	m_CommandList->IASetIndexBuffer(&IBView);
}

inline void GraphicsContext::SetVertexBuffer(UINT Slot, const D3D12_VERTEX_BUFFER_VIEW& VBView)
{
	m_CommandList->IASetVertexBuffers(Slot, 1, &VBView);
}

inline void GraphicsContext::DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation)
{
	FlushResourceBarrier();
	m_DynamicViewDescriptorHeap.CommitGraphicsRootDescriptorTables(m_CommandList);
	m_DynamicSamplerDescriptorHeap.CommitGraphicsRootDescriptorTables(m_CommandList);
	m_CommandList->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

inline void GraphicsContext::DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
{
	FlushResourceBarrier();
	m_DynamicViewDescriptorHeap.CommitGraphicsRootDescriptorTables(m_CommandList);
	m_DynamicSamplerDescriptorHeap.CommitGraphicsRootDescriptorTables(m_CommandList);
	m_CommandList->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}

class ComputeContext : public CommandContext
{
public:
	static ComputeContext& Begin(const std::wstring& id = L"", bool async = false);
};