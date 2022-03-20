#include "CommandAllocatorPool.h"

CommandAllocatorPool::CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE Type)
	:
	m_Device(nullptr),
	m_CommandListType(Type)
{
}

CommandAllocatorPool::~CommandAllocatorPool()
{
	Shutdown();
}

void CommandAllocatorPool::Initialize(ID3D12Device* pDevice)
{
	m_Device = pDevice;
}

void CommandAllocatorPool::Shutdown()
{
	for (auto* alloc : m_AllocatorPool)
	{
		alloc->Release();
	}
	m_AllocatorPool.clear();
	m_Device = nullptr;
}

ID3D12CommandAllocator* CommandAllocatorPool::GetAllocator(uint64_t fence)
{
	std::lock_guard<std::mutex> lockGuard(m_AllocatorMutex);

	ID3D12CommandAllocator* allocator = nullptr;

	if (!m_ReadyAllocators.empty())
	{
		auto pair = m_ReadyAllocators.front();

		// 此allocator已经使用完毕
		if (pair.first <= fence)
		{
			allocator = pair.second;
			ThrowIfFailed(allocator->Reset());
			m_ReadyAllocators.pop();
		}
	}

	if (allocator == nullptr)
	{
		ThrowIfFailed(m_Device->CreateCommandAllocator(m_CommandListType, IID_PPV_ARGS(&allocator)));
		allocator->SetName((std::wstring(L"CommandAllocator ") + std::to_wstring(m_AllocatorPool.size())).c_str());
		m_AllocatorPool.push_back(allocator);

		Utility::Print("New allocator!!!!\n");
	}

	return allocator;
}

void CommandAllocatorPool::ReturnAllocator(uint64_t fence, ID3D12CommandAllocator* allocator)
{
	std::lock_guard<std::mutex> lockGuard(m_AllocatorMutex);

	m_ReadyAllocators.push({ fence, allocator });
}
