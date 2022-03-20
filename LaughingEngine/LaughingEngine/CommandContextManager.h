#pragma once
#include "PipelineState.h"
#include <vector>
#include <queue>
#include <mutex>

/// <summary>
/// �������д���������Context
/// sm_ContextPool��Context�أ�����������ʹ�õĺ�û����ʹ�õģ����黹��
/// sm_AvailableContexts��������û����ʹ�õģ����黹��
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