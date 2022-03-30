#include "CommandContextManager.h"
#include "CommandContext.h"

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

void ContextManager::Shutdown()
{
	MemoryAllocator::DestroyAll();
	for (uint32_t i = 0; i < 4; i++)
	{
		sm_ContextPool[i].clear();
	}
}