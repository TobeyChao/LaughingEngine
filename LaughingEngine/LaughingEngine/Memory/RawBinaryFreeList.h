#pragma once
#include <cstdint>

template<typename GrowthPolicy, typename AllocationPolicy>
class RawBinaryFreeList : public GrowthPolicy, public AllocationPolicy
{
public:
	RawBinaryFreeList();
	RawBinaryFreeList(const RawBinaryFreeList&) = delete;
	RawBinaryFreeList(RawBinaryFreeList&&) = delete;
	RawBinaryFreeList& operator=(const RawBinaryFreeList&) = delete;
	RawBinaryFreeList& operator=(RawBinaryFreeList&&) = delete;
	~RawBinaryFreeList() = default;

	void* Allocate();
	void Free(void* pData);
};

template<typename GrowthPolicy, typename AllocationPolicy>
inline RawBinaryFreeList<GrowthPolicy, AllocationPolicy>::RawBinaryFreeList()
{
	uint32_t numToPrealloc = GrowthPolicy::GetNumToPreAllocate();
	if (numToPrealloc > 0)
	{
		AllocationPolicy::Grow(numToPrealloc);
	}
}

template<typename GrowthPolicy, typename AllocationPolicy>
inline void* RawBinaryFreeList<GrowthPolicy, AllocationPolicy>::Allocate()
{
	void* pBlock = AllocationPolicy::Pop();
	if (!pBlock)
	{
		uint32_t numAlloced = AllocationPolicy::GetNumAllocated();
		uint32_t growSize = GrowthPolicy::GetNumToGrow(numAlloced);
		if (growSize > 0)
		{
			AllocationPolicy::Grow(growSize);
			pBlock = AllocationPolicy::Pop();
		}
	}
	if (pBlock)
	{
		return AllocationPolicy::Create(pBlock);
	}
	return nullptr;
}

template<typename GrowthPolicy, typename AllocationPolicy>
inline void RawBinaryFreeList<GrowthPolicy, AllocationPolicy>::Free(void* pData)
{
	if (!pData)
	{
		return;
	}
	AllocationPolicy::Destroy(pData);
	AllocationPolicy::Push(pData);
}