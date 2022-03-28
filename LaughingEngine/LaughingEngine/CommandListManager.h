#pragma once
#include "PCH.h"
#include "CommandAllocatorPool.h"

class CommandQueue
{
public:
	CommandQueue(D3D12_COMMAND_LIST_TYPE Type);
	~CommandQueue();

	void Initialize(ID3D12Device* pDevice);
	void Shutdown();

	uint64_t IncrementFence();
	bool IsFenceComplete(uint64_t fenceValue);
	void WaitForFence(uint64_t fenceValue);
	void WaitForIdle(void) { WaitForFence(IncrementFence()); }

	ID3D12CommandQueue* GetCommandQueue()
	{
		return m_CommandQueue.Get();
	}

	uint64_t ExecuteCommandList(ID3D12CommandList* list);
	ID3D12CommandAllocator* RequestAllocator();
	void DiscardAllocator(uint64_t fenceValueForReset, ID3D12CommandAllocator* allocator);

private:
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;

	const D3D12_COMMAND_LIST_TYPE m_Type;

	CommandAllocatorPool m_CommandAllocatorPool;
	std::mutex m_FenceMutex;
	std::mutex m_EventMutex;

	Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;

	uint64_t m_NextFenceValue;
	uint64_t m_LastCompletedFenceValue;
	HANDLE m_FenceEventHandle;
};

class CommandListManager
{
public:
	CommandListManager();
	~CommandListManager();

	void Initialize(ID3D12Device* pDevice);
	void Shutdown();

	CommandQueue& GetGraphicsQueue() { return m_GraphicsQueue; }

	CommandQueue& GetQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT)
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			return m_GraphicsQueue;
		default:
			return m_GraphicsQueue;
		}
	}

	ID3D12CommandQueue* GetCommandQueue()
	{
		return m_GraphicsQueue.GetCommandQueue();
	}

	void CreateNewCommandList(
		D3D12_COMMAND_LIST_TYPE type,
		ID3D12GraphicsCommandList** list,
		ID3D12CommandAllocator** allocator);

	bool IsFenceComplete(uint64_t FenceValue)
	{
		return GetQueue(D3D12_COMMAND_LIST_TYPE(FenceValue >> 56)).IsFenceComplete(FenceValue);
	}

	void WaitForFence(uint64_t fenceValue);

	void IdleGPU()
	{
		m_GraphicsQueue.WaitForIdle();
	}

private:
	ID3D12Device* m_Device;
	CommandQueue m_GraphicsQueue;
};