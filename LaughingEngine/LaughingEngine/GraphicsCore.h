#pragma once

#include "PCH.h"
#include "DescriptorHeap.h"

class CommandListManager;
class ContextManager;

namespace Graphics
{
	void Initialize();
	void Shutdown();

	extern ID3D12Device* g_Device;
	extern CommandListManager g_CommandManager;
	extern ContextManager g_ContextManager;

	extern DescriptorAllocator g_DescriptorAllocator[];
	inline D3D12_CPU_DESCRIPTOR_HANDLE AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t count = 1u)
	{
		return g_DescriptorAllocator[type].Alloc(count);
	}
}