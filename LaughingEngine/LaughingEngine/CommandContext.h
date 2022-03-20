#pragma once

#include "pch.h"
#include "CommandListManager.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "PixelBuffer.h"
#include "ColorBuffer.h"

#include <vector>
#include <queue>
#include <mutex>
#include <string>

class GraphicsContext;
class ComputeContext;

/// <summary>
/// 管理所有创建出来的Context
/// sm_ContextPool是Context池，包含了正在使用的和没有在使用的，即归还的
/// sm_AvailableContexts，包含了没有在使用的，即归还的
/// </summary>
class ContextManager
{
public:
	ContextManager() = default;
	CommandContext* AllocateContext(D3D12_COMMAND_LIST_TYPE type);
	void ReturnContext(CommandContext* commandContext);
	void Shutdown() { DestroyAllContext(); }

private:
	void DestroyAllContext();

private:
	std::vector<std::unique_ptr<CommandContext>> sm_ContextPool[4];
	std::queue<CommandContext*> sm_AvailableContexts[4];
	std::mutex sm_ContextAllocationMutex;
};

class CommandContext
{
	friend ContextManager;

public:
	CommandContext(D3D12_COMMAND_LIST_TYPE Type);
	~CommandContext();

	void Initialize();
	void Reset();

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

	D3D12_RESOURCE_BARRIER m_ResourceBarrier[16];
	UINT m_NumBarriers;

	ID3D12DescriptorHeap* m_DescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

	std::wstring m_ID;
	D3D12_COMMAND_LIST_TYPE m_Type;
};

class GraphicsContext : public CommandContext
{
public:
	static GraphicsContext& Begin(const std::wstring& id = L"");

	void ClearColor(ColorBuffer& Target, const D3D12_RECT* Rect);
	void SetRootSignature(const RootSignature& rootSignature);

	void SetRenderTargets(UINT numRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE rtv[]);
	void SetRenderTargets(UINT numRTVs, const D3D12_CPU_DESCRIPTOR_HANDLE rtv[], D3D12_CPU_DESCRIPTOR_HANDLE dsv);
	void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv) { SetRenderTargets(1, &rtv); }
	void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv) { SetRenderTargets(1, &rtv, dsv); }
	void SetDepthStencilTarget(D3D12_CPU_DESCRIPTOR_HANDLE dsv) { SetRenderTargets(0, nullptr, dsv); }

	void SetViewport(const D3D12_VIEWPORT& Viewport);
	void SetScissorRect(const D3D12_RECT& Rect);
	void SetViewportAndScissorRect(const D3D12_VIEWPORT& Viewport, const D3D12_RECT& Rect);
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology);

	void SetDescriptorTable(UINT RootIndex, D3D12_GPU_DESCRIPTOR_HANDLE FirstHandle);

	void DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation = 0, UINT StartInstanceLocation = 0);
	void DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation);
};

inline void CommandContext::SetPipelineState(const PipelineState& pso)
{
	m_CommandList->SetPipelineState(pso.GetPSO());
}

inline void GraphicsContext::SetRootSignature(const RootSignature& rootSignature)
{
	m_CommandList->SetGraphicsRootSignature(rootSignature.GetRootSignature());
}

inline void GraphicsContext::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology)
{
	m_CommandList->IASetPrimitiveTopology(PrimitiveTopology);
}

inline void GraphicsContext::SetDescriptorTable(UINT RootIndex, D3D12_GPU_DESCRIPTOR_HANDLE FirstHandle)
{
	m_CommandList->SetGraphicsRootDescriptorTable(RootIndex, FirstHandle);
}

inline void GraphicsContext::DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation)
{
	FlushResourceBarrier();
	m_CommandList->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

inline void GraphicsContext::DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
{
	FlushResourceBarrier();
	m_CommandList->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}

class ComputeContext : public CommandContext
{
public:
	static ComputeContext& Begin(const std::wstring& id = L"", bool async = false);
};