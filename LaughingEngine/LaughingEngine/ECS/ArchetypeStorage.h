#ifndef ARCHETYPESTORAGE_
#define ARCHETYPESTORAGE_
#include "Memory/DefaultMultiTypeFixedChunk.h"
#include <memory>

class ArchetypeStorage
{
public:
	using PoolType = typename DefaultMultiTypeFixedChunk::Type;

	PoolType& Get(size_t index)
	{
		return *m_Pools[index];
	}

	void DestroyAll()
	{
		for (auto& pool : m_Pools)
		{
			pool.reset();
		}
		m_Pools.clear();
	}

	size_t GetIndex(size_t hash)
	{
		return m_HashToIndex[hash];
	}

	PoolType& AddPool(size_t hash)
	{
		if (!m_HashToIndex.contains(hash))
		{
			m_Pools.push_back(std::make_shared<PoolType>());
			m_HashToIndex[hash] = m_Pools.size() - 1;
		}
		return *m_Pools[m_HashToIndex[hash]];
	}

private:
	std::unordered_map<size_t, size_t> m_HashToIndex;
	std::vector<std::shared_ptr<PoolType>> m_Pools;
};
#endif // ARCHETYPESTORAGE_
