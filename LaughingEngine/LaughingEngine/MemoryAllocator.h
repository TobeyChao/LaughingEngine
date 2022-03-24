#pragma once
#include "GpuResource.h"

#include <vector>
#include <queue>
#include <mutex>

enum class AllocatorType
{
	Invalid = -1,
	Default = 0,	// Ĭ�϶ѣ�GPU�ɶ�
	Upload = 1,		// �ϴ��ѣ�CPU�ɶ�д
	NumTypes
};

struct MemoryHandle
{
	GpuResource& Res;
	size_t Offset;
	size_t BufferSize;
	void* CpuAddress;
	D3D12_GPU_VIRTUAL_ADDRESS GpuAddress;
};

class MemoryPage
{
public:

private:

};

class MemoryPageManager
{
public:

private:

};

class MemoryAllocator
{
public:

private:

};