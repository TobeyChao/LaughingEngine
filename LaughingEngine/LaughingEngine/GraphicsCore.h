#pragma once
#include "PCH.h"

class CommandListManager;
class ContextManager;

namespace Graphics
{
	void Initialize();
	void Shutdown();

	extern ID3D12Device* g_Device;
	extern CommandListManager g_CommandManager;
	extern ContextManager g_ContextManager;
}