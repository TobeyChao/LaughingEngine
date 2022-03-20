#include "CommandContext.h"
#include "GraphicsCore.h"

using namespace Graphics;

CommandContext* ContextManager::AllocateContext(D3D12_COMMAND_LIST_TYPE Type)
{
	std::lock_guard<std::mutex> lockGuarad(sm_ContextAllocationMutex);

	auto& availableContexts = sm_AvailableContexts[Type];

	CommandContext* ret = nullptr;
	if (availableContexts.empty())
	{
		ret = new CommandContext(Type);
		sm_ContextPool[Type].emplace_back(ret);
		ret->Initialize();
	}
	else
	{
		ret = availableContexts.front();
		availableContexts.pop();
		ret->Reset();
	}

	return ret;
}

void ContextManager::ReturnContext(CommandContext* CommandContext)
{
	assert(CommandContext != nullptr);
	std::lock_guard<std::mutex> lockGuarad(sm_ContextAllocationMutex);
	auto& availableContexts = sm_AvailableContexts[CommandContext->m_Type];
	availableContexts.push(CommandContext);
}

void ContextManager::DestroyAllContext()
{
	for (uint32_t i = 0; i < 4; i++)
	{
		sm_ContextPool[i].clear();
	}
}

CommandContext::CommandContext(D3D12_COMMAND_LIST_TYPE Type)
	:
	m_Type(Type)
{
}

CommandContext::~CommandContext()
{
	if (m_CommandList != nullptr)
		m_CommandList->Release();
}

void CommandContext::Initialize()
{
	g_CommandManager.CreateNewCommandList(m_Type, &m_CommandList, &m_Allocator);
}

void CommandContext::Reset()
{
	assert(m_CommandList != nullptr && m_Allocator == nullptr);
	m_Allocator = g_CommandManager.GetQueue(m_Type).RequestAllocator();
	m_CommandList->Reset(m_Allocator, nullptr);

	m_GraphicsRootSignature = nullptr;
	m_ComputeRootSignature = nullptr;
	m_PipelineState = nullptr;
	m_NumBarriers = 0;

	BindDescriptorHeaps();
}

uint64_t CommandContext::Flush(bool WaitForCompletion)
{
	FlushResourceBarrier();

	uint64_t fenceValue = g_CommandManager.GetQueue(m_Type).ExecuteCommandList(m_CommandList);

	if (WaitForCompletion)
	{
		g_CommandManager.WaitForFence(fenceValue);
	}

	m_CommandList->Reset(m_Allocator, nullptr);

	if (m_GraphicsRootSignature)
	{
		m_CommandList->SetGraphicsRootSignature(m_GraphicsRootSignature);
	}
	if (m_ComputeRootSignature)
	{
		m_CommandList->SetComputeRootSignature(m_ComputeRootSignature);
	}
	if (m_PipelineState)
	{
		m_CommandList->SetPipelineState(m_PipelineState);
	}

	BindDescriptorHeaps();

	return fenceValue;
}

uint64_t CommandContext::Finish(bool WaitForCompletion)
{
	FlushResourceBarrier();

	CommandQueue& Queue = g_CommandManager.GetQueue(m_Type);

	uint64_t fenceValue = Queue.ExecuteCommandList(m_CommandList);
	Queue.DiscardAllocator(fenceValue, m_Allocator);
	m_Allocator = nullptr;

	if (WaitForCompletion)
	{
		g_CommandManager.WaitForFence(fenceValue);
	}

	g_ContextManager.ReturnContext(this);

	return fenceValue;
}

void CommandContext::TransitionResource(GpuResource& res, D3D12_RESOURCE_STATES targetState, bool immediate)
{
	D3D12_RESOURCE_STATES oldState = res.CurState();
	if (oldState != targetState)
	{
		assert(m_NumBarriers < 16);

		m_ResourceBarrier[m_NumBarriers] = CD3DX12_RESOURCE_BARRIER::Transition(
			res.GetRes(),
			oldState,
			targetState,
			D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
			D3D12_RESOURCE_BARRIER_FLAG_NONE);

		if (targetState == res.TargetState())
		{
			m_ResourceBarrier[m_NumBarriers].Flags = D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
			res.SetStateTarget((D3D12_RESOURCE_STATES)-1);
		}
		res.SetState(targetState);

		m_NumBarriers++;
	}

	if (immediate || m_NumBarriers == 16)
	{
		FlushResourceBarrier();
	}
}

void CommandContext::FlushResourceBarrier()
{
	if (m_NumBarriers > 0)
	{
		m_CommandList->ResourceBarrier(m_NumBarriers, m_ResourceBarrier);
		m_NumBarriers = 0;
	}
}

void CommandContext::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12DescriptorHeap* heap)
{
	if (m_DescriptorHeaps[type] != heap)
	{
		m_DescriptorHeaps[type] = heap;
		BindDescriptorHeaps();
	}
}

void CommandContext::SetDescriptorHeaps(UINT heapCount, D3D12_DESCRIPTOR_HEAP_TYPE type[], ID3D12DescriptorHeap* heap[])
{
	bool flag = false;

	for (UINT i = 0; i < heapCount; i++)
	{
		if (m_DescriptorHeaps[type[i]] != heap[i])
		{
			m_DescriptorHeaps[type[i]] = heap[i];
			flag = true;
		}
	}

	if (flag)
	{
		BindDescriptorHeaps();
	}
}

void CommandContext::BindDescriptorHeaps()
{
	UINT count = 0;
	ID3D12DescriptorHeap* heaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
	for (UINT i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++)
	{
		ID3D12DescriptorHeap* heap = m_DescriptorHeaps[i];
		if (heap != nullptr)
		{
			heaps[count] = heap;
			++count;
		}
	}
	if (count > 0)
	{
		m_CommandList->SetDescriptorHeaps(count, heaps);
	}
}

GraphicsContext& GraphicsContext::Begin(const std::wstring& id)
{
	CommandContext* context = g_ContextManager.AllocateContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
	context->SetID(id);
	return context->GetGraphicsContext();
}

void GraphicsContext::ClearColor(ColorBuffer& Target, const D3D12_RECT* Rect)
{
	FlushResourceBarrier();
	m_CommandList->ClearRenderTargetView(Target.GetRTV(), Target.GetClearColor(), (Rect == nullptr) ? 0 : 1, Rect);
}

void GraphicsContext::SetRenderTargets(
	UINT NumRenderTargetDescriptors,
	const D3D12_CPU_DESCRIPTOR_HANDLE pRenderTargetDescriptors[])
{
	m_CommandList->OMSetRenderTargets(
		NumRenderTargetDescriptors,
		pRenderTargetDescriptors,
		FALSE,
		nullptr);
}

void GraphicsContext::SetRenderTargets(UINT NumRenderTargetDescriptors,
	const D3D12_CPU_DESCRIPTOR_HANDLE pRenderTargetDescriptors[],
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilDescriptor)
{
	m_CommandList->OMSetRenderTargets(NumRenderTargetDescriptors,
		pRenderTargetDescriptors,
		FALSE,
		&DepthStencilDescriptor);
}

void GraphicsContext::SetViewport(const D3D12_VIEWPORT& Viewport)
{
	m_CommandList->RSSetViewports(1, &Viewport);
}

void GraphicsContext::SetScissorRect(const D3D12_RECT& Rect)
{
	m_CommandList->RSSetScissorRects(1, &Rect);
}

void GraphicsContext::SetViewportAndScissorRect(const D3D12_VIEWPORT& Viewport, const D3D12_RECT& Rect)
{
	m_CommandList->RSSetViewports(1, &Viewport);
	m_CommandList->RSSetScissorRects(1, &Rect);
}

ComputeContext& ComputeContext::Begin(const std::wstring& id, bool async)
{
	CommandContext* context = g_ContextManager.AllocateContext(
		async ? D3D12_COMMAND_LIST_TYPE_COMPUTE : D3D12_COMMAND_LIST_TYPE_DIRECT);
	context->SetID(id);
	return context->GetComputeContext();
}