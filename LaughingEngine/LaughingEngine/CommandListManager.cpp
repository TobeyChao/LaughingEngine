#include "CommandListManager.h"
#include "GraphicsCore.h"

CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE type)
	:
	m_Type(type),
	m_CommandAllocatorPool(type),
	m_NextFenceValue((uint64_t)type << 56 | 1),
	m_LastCompletedFenceValue((uint64_t)type << 56),
	m_FenceEventHandle(NULL)
{
}

CommandQueue::~CommandQueue()
{
	Shutdown();
}

void CommandQueue::Initialize(ID3D12Device* pDevice)
{
	// 创建命令队列
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 1;
	queueDesc.Type = m_Type;
	ThrowIfFailed(pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_CommandQueue.GetAddressOf())));
	m_CommandQueue->SetName(L"CommandListManager::m_CommandQueue");

	// 创建围栏
	ThrowIfFailed(pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
	m_Fence->SetName(L"CommandListManager::m_Fence");
	m_Fence->Signal((uint64_t)m_Type << 56);
	// 创建围栏事件
	m_FenceEventHandle = CreateEvent(nullptr, false, false, nullptr);
	// 初始化命令分配器池
	m_CommandAllocatorPool.Initialize(pDevice);
}

void CommandQueue::Shutdown()
{
	if (m_CommandQueue == nullptr)
	{
		return;
	}

	m_CommandAllocatorPool.Shutdown();

	CloseHandle(m_FenceEventHandle);

	m_Fence.Reset();

	m_CommandQueue.Reset();
}

uint64_t CommandQueue::IncrementFence()
{
	std::lock_guard<std::mutex> lockGuard(m_FenceMutex);
	m_CommandQueue->Signal(m_Fence.Get(), m_NextFenceValue);
	return m_NextFenceValue++;
}

bool CommandQueue::IsFenceComplete(uint64_t fenceValue)
{
	if (fenceValue > m_LastCompletedFenceValue)
	{
		m_LastCompletedFenceValue = std::max(m_LastCompletedFenceValue, m_Fence->GetCompletedValue());
	}
	return fenceValue <= m_LastCompletedFenceValue;
}

void CommandQueue::WaitForFence(uint64_t fenceValue)
{
	if (IsFenceComplete(fenceValue))
	{
		return;
	}
	std::lock_guard<std::mutex> lockGuard(m_EventMutex);
	m_Fence->SetEventOnCompletion(fenceValue, m_FenceEventHandle);
	WaitForSingleObject(m_FenceEventHandle, INFINITE);
	m_LastCompletedFenceValue = fenceValue;
}

uint64_t CommandQueue::ExecuteCommandList(ID3D12CommandList* list)
{
	std::lock_guard<std::mutex> lockGuard(m_FenceMutex);
	
	ID3D12GraphicsCommandList* graphicList = (ID3D12GraphicsCommandList*)list;

	ThrowIfFailed(graphicList->Close());

	m_CommandQueue->ExecuteCommandLists(1, &list);

	m_CommandQueue->Signal(m_Fence.Get(), m_NextFenceValue);

	return m_NextFenceValue++;
}

ID3D12CommandAllocator* CommandQueue::RequestAllocator()
{
	uint64_t completedFence = m_Fence->GetCompletedValue();
	return m_CommandAllocatorPool.GetAllocator(completedFence);
}

void CommandQueue::DiscardAllocator(uint64_t fenceValueForReset, ID3D12CommandAllocator* allocator)
{
	m_CommandAllocatorPool.ReturnAllocator(fenceValueForReset, allocator);
}

CommandListManager::CommandListManager()
	:
	m_Device(nullptr),
	m_GraphicsQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)
{
}

CommandListManager::~CommandListManager()
{
	Shutdown();
}

void CommandListManager::Initialize(ID3D12Device* pDevice)
{
	m_Device = pDevice;

	m_GraphicsQueue.Initialize(pDevice);
}

void CommandListManager::Shutdown()
{
	m_GraphicsQueue.Shutdown();

	m_Device = nullptr;
}

void CommandListManager::CreateNewCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12GraphicsCommandList** list, ID3D12CommandAllocator** allocator)
{
	switch (type)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
		*allocator = m_GraphicsQueue.RequestAllocator();
		break;
	default:
		*allocator = m_GraphicsQueue.RequestAllocator();
		break;
	}

	ThrowIfFailed(m_Device->CreateCommandList(1, type, *allocator, nullptr, IID_PPV_ARGS(list)));
	(*list)->SetName(L"CommandList");
}

void CommandListManager::WaitForFence(uint64_t fenceValue)
{
	CommandQueue& Producer = Graphics::g_CommandManager.GetQueue((D3D12_COMMAND_LIST_TYPE)(fenceValue >> 56));
	Producer.WaitForFence(fenceValue);
}