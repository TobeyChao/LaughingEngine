#ifndef PLACEMENTNEWEMBEDDEDLINK_
#define PLACEMENTNEWEMBEDDEDLINK_
#include <vector>
#include <algorithm>

template<typename T>
class PlacementNewEmbeddedLink
{
public:
	void Grow(uint32_t numBlocks);

	void Push(void* pBlock);
	void* Pop();

	uint32_t GetNumAllocated() const;

	static T* Create(void* pBlock);
	static void Destroy(T* pBlock);

private:
	struct FreeBlock
	{
		FreeBlock* Next;
	};
	// 空闲内存块链表头节点
	FreeBlock* m_pFreeBlocks;

	std::vector<void*> m_Chunks;

	// 因为使用块的前sizeof(char*)个字节作为链接下一个块的指针，所以最小是sizeof(char*)
	constexpr static size_t BlockSize = std::max(sizeof(T), sizeof(char*));
};

template<typename T>
inline void PlacementNewEmbeddedLink<T>::Grow(uint32_t numBlocks)
{
	void* pChunk = malloc(numBlocks * BlockSize);
	m_Chunks.push_back(pChunk);
	for (uint32_t i = 0; i < numBlocks; i++)
	{
		Push((char*)pChunk + i * BlockSize);
	}
}

template<typename T>
inline void PlacementNewEmbeddedLink<T>::Push(void* pBlock)
{
	FreeBlock* pNewHead = (FreeBlock*)pBlock;
	pNewHead->Next = m_pFreeBlocks;
	m_pFreeBlocks = pNewHead;
}

template<typename T>
inline void* PlacementNewEmbeddedLink<T>::Pop()
{
	if (!m_pFreeBlocks)
	{
		return nullptr;
	}
	void* pNewBlock = m_pFreeBlocks;
	m_pFreeBlocks = m_pFreeBlocks->Next;
	return pNewBlock;
}

template<typename T>
inline uint32_t PlacementNewEmbeddedLink<T>::GetNumAllocated() const
{
	return (uint32_t)m_Chunks.size();
}

template<typename T>
inline T* PlacementNewEmbeddedLink<T>::Create(void* pBlock)
{
	return new(pBlock)T;
}

template<typename T>
inline void PlacementNewEmbeddedLink<T>::Destroy(T* pObject)
{
	pObject->~T();
}
#endif // PLACEMENTNEWEMBEDDEDLINK_
