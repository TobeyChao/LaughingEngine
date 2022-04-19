#include "PCH.h"
#include "MemoryAllocator.h"
#include "CommandListManager.h"
#include "GraphicsCore.h"

AllocatorType MemoryPageManager::sm_Type = AllocatorType::Default;

MemoryPageManager::MemoryPageManager()
{
	m_AllocationType = sm_Type;
	sm_Type = (AllocatorType)((size_t)sm_Type + 1);
	assert(sm_Type <= AllocatorType::NumTypes);
}

MemoryPage* MemoryPageManager::Get()
{
	std::lock_guard<std::mutex> LockGuard(m_Mutex);
	while (!m_MemoryPagesToReturn.empty() && Graphics::g_CommandManager.IsFenceComplete(m_MemoryPagesToReturn.front().first))
	{
		m_AvailablePages.push(m_MemoryPagesToReturn.front().second);
		m_MemoryPagesToReturn.pop();
	}

	MemoryPage* PagePtr = nullptr;

	if (!m_AvailablePages.empty())
	{
		PagePtr = m_AvailablePages.front();
		m_AvailablePages.pop();
	}
	else
	{
		PagePtr = CreatePage();
		m_PagePool.emplace_back(PagePtr);
	}

	return PagePtr;
}

MemoryPage* MemoryPageManager::CreatePage(size_t PageSize)
{
	CD3DX12_HEAP_PROPERTIES Prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC Desc = CD3DX12_RESOURCE_DESC::Buffer(PageSize);
	D3D12_RESOURCE_STATES DefaultUsage;
	ZeroMemory(&DefaultUsage, sizeof(DefaultUsage));

	if (m_AllocationType == AllocatorType::Default)
	{
		Prop.Type = D3D12_HEAP_TYPE_DEFAULT;
		Desc.Width = PageSize == 0 ? GpuMemoryPageSize : PageSize;
		Desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		DefaultUsage = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	}
	else
	{
		Prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		Desc.Width = PageSize == 0 ? CpuMemoryPageSize : PageSize;
		Desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		DefaultUsage = D3D12_RESOURCE_STATE_GENERIC_READ;
	}

	ID3D12Resource* pBuffer = nullptr;

	ThrowIfFailed(Graphics::g_Device->CreateCommittedResource(
		&Prop,
		D3D12_HEAP_FLAG_NONE,
		&Desc,
		DefaultUsage,
		nullptr,
		IID_PPV_ARGS(&pBuffer)));

	pBuffer->SetName(L"LinearAllocator Page");

	return new MemoryPage(pBuffer, DefaultUsage);
}

void MemoryPageManager::ReturnPages(uint64_t FenceID, const std::vector<MemoryPage*> Pages)
{
	std::lock_guard<std::mutex> LockGuard(m_Mutex);
	for (size_t i = 0; i < Pages.size(); i++)
	{
		m_MemoryPagesToReturn.push({ FenceID,  Pages[i] });
	}
}

void MemoryPageManager::DestroyLargePages(uint64_t FenceID, const std::vector<MemoryPage*> Pages)
{
	std::lock_guard<std::mutex> LockGuard(m_Mutex);
	while (!m_DeletionQueue.empty() && Graphics::g_CommandManager.IsFenceComplete(m_DeletionQueue.front().first))
	{
		delete m_DeletionQueue.front().second;
		m_DeletionQueue.pop();
	}

	for (size_t i = 0; i < Pages.size(); i++)
	{
		Pages[i]->Unmap();
		m_DeletionQueue.push({ FenceID, Pages[i] });
	}
}

void MemoryPageManager::DestroyAll()
{
	while (!m_MemoryPagesToReturn.empty())
	{
		m_MemoryPagesToReturn.pop();
	}
	m_PagePool.clear();
}

MemoryPageManager MemoryAllocator::sm_MemoryPageManager[2];

MemoryHandle MemoryAllocator::Allocate(size_t SizeInBytes, size_t Alignment)
{
	// 0.检查Alignment是不是2的n次幂
	// 例如16: 10000 & 01111 = 0
	assert((Alignment & (Alignment - 1)) == 0);

	// 1.字节对其
	const size_t AlignedSize = Math::AlignUp(SizeInBytes, Alignment);

	// 2.Size大于规范，使用大内存页
	if (AlignedSize > m_PageSize)
	{
		MemoryPage* page = sm_MemoryPageManager[m_Type].CreatePage(AlignedSize);
		m_LargeMemoryPagesToReturn.push_back(page);

		MemoryHandle handle(*page, 0, AlignedSize);
		// m_CurOffset为偏移字节数
		handle.CpuAddress = page->GetCpuAddress();
		handle.GpuAddress = page->GetGpuAddress();

		return handle;
	}

	// 3.Size小于规范，使用规范大小的内存页
	// 当前页空间不够了，需要申请新的
	m_CurOffset = Math::AlignUp(m_CurOffset, Alignment);
	if (m_CurOffset + AlignedSize > m_PageSize)
	{
		m_MemoryPagesToReturn.push_back(m_CurPage);
		m_CurPage = nullptr;
	}

	if (m_CurPage == nullptr)
	{
		m_CurPage = sm_MemoryPageManager[m_Type].Get();
		m_CurOffset = 0;
	}

	MemoryHandle handle(*m_CurPage, m_CurOffset, AlignedSize);
	// m_CurOffset为偏移字节数
	handle.CpuAddress = (uint8_t*)m_CurPage->GetCpuAddress() + m_CurOffset;
	handle.GpuAddress = m_CurPage->GetGpuAddress() + m_CurOffset;

	m_CurOffset += AlignedSize;

	return handle;
}

void MemoryAllocator::CleanupUsedPages(uint64_t FenceID)
{
	if (!m_CurPage)
	{
		return;
	}

	m_MemoryPagesToReturn.push_back(m_CurPage);
	m_CurPage = nullptr;
	m_CurOffset = 0;

	sm_MemoryPageManager[m_Type].ReturnPages(FenceID, m_MemoryPagesToReturn);
	m_MemoryPagesToReturn.clear();

	sm_MemoryPageManager[m_Type].DestroyLargePages(FenceID, m_LargeMemoryPagesToReturn);
	m_LargeMemoryPagesToReturn.clear();
}