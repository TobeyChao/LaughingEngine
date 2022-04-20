#pragma once
#include <cstdint>

template<typename T, typename GrowthPolicy, typename AllocationPolicy>
class FreeList : public GrowthPolicy, public AllocationPolicy
{
public:
	FreeList();
	FreeList(const FreeList&) = delete;
	FreeList(FreeList&&) = delete;
	FreeList& operator=(const FreeList&) = delete;
	FreeList& operator=(FreeList&&) = delete;
	~FreeList() = default;

	T* Allocate();
	void Free(T* pObject);
};

template<typename T, typename GrowthPolicy, typename AllocationPolicy>
inline FreeList<T, GrowthPolicy, AllocationPolicy>::FreeList()
{
	uint32_t numToPrealloc = GrowthPolicy::GetNumToPreAllocate();
	if (numToPrealloc > 0)
	{
		AllocationPolicy::Grow(numToPrealloc);
	}
}

template<typename T, typename GrowthPolicy, typename AllocationPolicy>
inline T* FreeList<T, GrowthPolicy, AllocationPolicy>::Allocate()
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

template<typename T, typename GrowthPolicy, typename AllocationPolicy>
inline void FreeList<T, GrowthPolicy, AllocationPolicy>::Free(T* pObject)
{
	if (!pObject)
	{
		return;
	}
	AllocationPolicy::Destroy(pObject);
	AllocationPolicy::Push(pObject);
}