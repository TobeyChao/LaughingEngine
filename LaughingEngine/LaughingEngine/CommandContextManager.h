#pragma once
#include "PipelineState.h"
#include <vector>
#include <queue>
#include <mutex>

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