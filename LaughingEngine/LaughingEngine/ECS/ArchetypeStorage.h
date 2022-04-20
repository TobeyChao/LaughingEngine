#pragma once
#include "Memory/DefaultMultiTypeFixedChunk.h"
#include <memory>

class ArchetypeStorage
{
public:
	using PoolType = typename DefaultMultiTypeFixedChunk::Type;

	PoolType& Get(size_t hash)
	{
		if (!m_Pools.contains(hash))
		{
			m_Pools[hash] = std::make_shared<PoolType>();
		}
		return *m_Pools[hash];
	}

	void DestroyAll()
	{
		for (auto& [_, pool] : m_Pools)
		{
			pool.reset();
		}
		m_Pools.clear();
	}

private:
	std::unordered_map<size_t, std::shared_ptr<PoolType>> m_Pools;
};