#pragma once
#include "GpuResource.h"

#include <vector>
#include <queue>
#include <mutex>

enum class AllocatorType
{
	Invalid = -1,
	Default = 0,	// 默认堆，GPU可读
	Upload = 1,		// 上传堆，CPU可读写
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