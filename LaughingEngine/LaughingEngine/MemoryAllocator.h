#pragma once
#include "GpuResource.h"
#include <vector>
#include <queue>
#include <mutex>

#define DEFAULT_ALIGN 256

enum AllocatorType
{
	Invalid = -1,
	Default = 0,	// 默认堆，GPU可读
	Upload = 1,		// 上传堆，CPU可读写
	NumTypes
};

enum AllocatorSize
{
	GpuMemoryPageSize = 0x10000,	// 64K
	CpuMemoryPageSize = 0x200000	// 2MB
};

struct MemoryHandle
{
	MemoryHandle(GpuResource& gpuRes, size_t Offset, size_t Size)
		:
		Res(gpuRes),
		Offset(Offset),
		BufferSize(Size),
		CpuAddress(nullptr),
		GpuAddress(0)
	{}
	GpuResource& Res;
	size_t Offset;
	size_t BufferSize;
	void* CpuAddress;
	D3D12_GPU_VIRTUAL_ADDRESS GpuAddress;
};

class MemoryPage : public GpuResource
{
public:
	MemoryPage(ID3D12Resource* pRes, D3D12_RESOURCE_STATES State)
		:
		GpuResource(pRes, State)
	{
		m_GpuVirtualAddress = m_pResource->GetGPUVirtualAddress();
		m_pResource->Map(0, nullptr, &CpuAddress);
	}

	virtual ~MemoryPage()
	{
		Unmap();
	}

	void Map()
	{
		if (!CpuAddress)
		{
			m_pResource->Map(0, nullptr, &CpuAddress);
		}
	}

	void Unmap()
	{
		if (CpuAddress)
		{
			m_pResource->Unmap(0, nullptr);
		}
	}

	void* GetCpuAddress() const
	{
		return CpuAddress;
	}

private:
	void* CpuAddress;
};

class MemoryPageManager
{
public:
	MemoryPageManager();
	MemoryPage* Get();
	MemoryPage* CreatePage(size_t PageSize = 0);
	void ReturnPages(uint64_t FenceID, const std::vector<MemoryPage*> Pages);
	void DestroyLargePages(uint64_t FenceID, const std::vector<MemoryPage*> Pages);
	void DestroyAll();

private:
	static AllocatorType sm_Type;

	AllocatorType m_AllocationType;
	std::vector<std::unique_ptr<MemoryPage> > m_PagePool;
	std::queue<std::pair<uint64_t, MemoryPage*> > m_MemoryPagesToReturn;
	std::queue<std::pair<uint64_t, MemoryPage*> > m_DeletionQueue;
	std::queue<MemoryPage*> m_AvailablePages;
	std::mutex m_Mutex;
};

class MemoryAllocator
{
public:
	MemoryAllocator(AllocatorType Type)
		:
		m_Type(Type),
		m_CurOffset(0),
		m_CurPage(nullptr)
	{
		m_PageSize = Type == AllocatorType::Default ? AllocatorSize::GpuMemoryPageSize : AllocatorSize::CpuMemoryPageSize;
	}

	MemoryHandle Allocate(size_t SizeInBytes, size_t Alignment = DEFAULT_ALIGN);

	void CleanupUsedPages(uint64_t FenceID);

	static void DestroyAll()
	{
		sm_MemoryPageManager[0].DestroyAll();
		sm_MemoryPageManager[1].DestroyAll();
	}

private:
	static MemoryPageManager sm_MemoryPageManager[2];
	// 类型
	AllocatorType m_Type;
	// 规范内存页大小
	size_t m_PageSize;
	// 当前内存偏移
	size_t m_CurOffset;
	// 当前内存页
	MemoryPage* m_CurPage;
	// 使用过的所有内存页
	std::vector<MemoryPage*> m_MemoryPagesToReturn;
	std::vector<MemoryPage*> m_LargeMemoryPagesToReturn;
};