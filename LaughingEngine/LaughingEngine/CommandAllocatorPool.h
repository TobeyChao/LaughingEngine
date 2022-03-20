#pragma once
#include "PCH.h"
#include <vector>
#include <queue>
#include <mutex>

class CommandAllocatorPool
{
public:
	CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE Type);
	~CommandAllocatorPool();

	void Initialize(ID3D12Device* pDevice);
	void Shutdown();

	ID3D12CommandAllocator* GetAllocator(uint64_t fence);
	void ReturnAllocator(uint64_t fence, ID3D12CommandAllocator* allocator);

private:
	const D3D12_COMMAND_LIST_TYPE m_CommandListType;

	ID3D12Device* m_Device;
	std::vector<ID3D12CommandAllocator*> m_AllocatorPool;
	std::queue<std::pair<uint64_t, ID3D12CommandAllocator*>> m_ReadyAllocators;
	std::mutex m_AllocatorMutex;
};