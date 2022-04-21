#ifndef FIXEDCHUNKCACHEFRIENDLYLINK_
#define FIXEDCHUNKCACHEFRIENDLYLINK_
#include <vector>
#include <stack>
#include <unordered_map>
#include "ChunkHandle.h"
#include "../MPL/TypeList.h"

#ifdef _DEBUG
#include <iostream>
#endif // DEBUG

class FixedChunkCacheFriendlyLink
{
private:
	template<typename T>
	struct TypeToIndex
	{
		static inline size_t Index = 0;
	};
protected:
	~FixedChunkCacheFriendlyLink()
	{
		for (auto& [_, handle] : m_Handles)
		{
			delete handle;
			handle = nullptr;
		}
		m_Handles.clear();

		m_UsedChunks.clear();
		while (!m_FreeChunks.empty())
		{
			m_FreeChunks.pop();
		}

		for (size_t i = 0; i < m_AllChunks.size(); i++)
		{
			free(m_AllChunks[i]);
		}
		m_AllChunks.clear();
	}

	template<typename TypeList>
	bool Init(uint64_t ChunkSize)
	{
		m_ElementSize = SizeSum<TypeList>::Value;
		m_ElementMaxCount = ChunkSize / m_ElementSize;
		_ParseDataStructure<TypeList>(std::make_index_sequence<Length<TypeList>()>());

#ifdef _DEBUG
		std::cout << "ElementSize: " << m_ElementSize << std::endl;
		std::cout << "ElementMaxCount: " << m_ElementMaxCount << std::endl;
		for (int i = 0; i < m_TypeOffset.size(); ++i)
		{
			auto& [offset, size] = m_TypeOffset[i];
			std::cout << "Index: " << i << " Addr: " << offset << " Length: " << size << std::endl;
		}
#endif // DEBUG
		return true;
	}

	void Grow(uint64_t SizeInBytes)
	{
		void* pChunk = nullptr;
		if (m_FreeChunks.empty())
		{
			pChunk = malloc(SizeInBytes);
			m_AllChunks.push_back(pChunk);
		}
		else
		{
			pChunk = m_FreeChunks.top();
			m_FreeChunks.pop();
		}
		m_UsedChunks.push_back(pChunk);
		m_CurIndex = 0;
	}

	ChunkHandle* Pop()
	{
		if (m_CurIndex >= m_ElementMaxCount)
		{
			return nullptr;
		}
		auto DataIndex = m_CurIndex++;
		auto ChunkIndex = m_UsedChunks.size() - 1;

		ChunkHandle* handle = new ChunkHandle();
		handle->DataIndex = DataIndex;
		handle->ChunkIndex = ChunkIndex;
		m_Handles[GetHandleKey(ChunkIndex, DataIndex)] = handle;
		return handle;
	}

	template<typename T, typename ...Args>
	T* Create(ChunkHandle* handle, Args&&... args)
	{
		void* chunk = m_UsedChunks[handle->ChunkIndex];
		uint64_t offset = std::get<0>(m_TypeOffset[TypeToIndex<T>::Index]);
		void* addr = (uint8_t*)chunk + offset + handle->DataIndex * sizeof(T);
		return new(addr)T(std::forward<decltype(args)>(args)...);
	}

	template<typename T>
	T* Get(ChunkHandle* handle)
	{
		void* chunk = m_UsedChunks[handle->ChunkIndex];
		uint64_t offset = std::get<0>(m_TypeOffset[TypeToIndex<T>::Index]);
		void* addr = (uint8_t*)chunk + offset + handle->DataIndex * sizeof(T);
		return reinterpret_cast<T*>(addr);
	}

	void Destroy(ChunkHandle* handle)
	{
		for (auto& [offset, size] : m_TypeOffset)
		{
			void* chunk = m_UsedChunks[handle->ChunkIndex];
			void* addr = (uint8_t*)chunk + offset + handle->DataIndex * size;
			// TODO 如何调用析构函数
		}
	}

	void Push(ChunkHandle* handle)
	{
		// 计算得到最后一个数据的Handle
		uint64_t TheLastChunkIndex = m_UsedChunks.size() - 1;
		uint64_t TheLastDataIndex = m_CurIndex - 1;
		uint64_t TheLastKey = GetHandleKey(TheLastChunkIndex, TheLastDataIndex);
		// 需要释放的
		uint64_t ToReleaseChunkIndex = handle->ChunkIndex;
		uint64_t ToReleaseDataIndex = handle->DataIndex;
		uint64_t ToReleaseKey = GetHandleKey(ToReleaseChunkIndex, ToReleaseDataIndex);

		for (auto& [offset, size] : m_TypeOffset)
		{
			void* chunk = m_UsedChunks[handle->ChunkIndex];
			void* theLastChunk = m_UsedChunks.back();
			uint64_t typeOffset = offset;
			uint64_t dataOffset = handle->DataIndex * size;
			uint64_t lastOffset = (m_CurIndex - 1) * size;
			void* dst = (uint8_t*)chunk + typeOffset + dataOffset;
			void* src = (uint8_t*)theLastChunk + typeOffset + lastOffset;
			if (src != dst)
			{
				memcpy(dst, src, size);
			}
			memset(src, 0, size);
		}

		// 更改最后一个数据的Chunk和Data的Index
		m_Handles[TheLastKey]->ChunkIndex = ToReleaseChunkIndex;
		m_Handles[TheLastKey]->DataIndex = ToReleaseDataIndex;
		m_Handles[ToReleaseKey] = m_Handles[TheLastKey];

		m_Handles.erase(TheLastKey);
		delete handle;
		handle = nullptr;
		m_CurIndex--;

		// 空的去掉
		if (m_CurIndex == 0)
		{
			m_CurIndex = m_ElementMaxCount;
			m_FreeChunks.push(m_UsedChunks.back());
			m_UsedChunks.pop_back();
		}
	}

private:
	template<typename TypeList, std::size_t... Is>
	constexpr void _ParseDataStructure(std::index_sequence<Is...> sequence)
	{
		uint64_t offset = 0;
		size_t index = 0;
		m_TypeOffset.resize(sequence.size());
		(_ParseDataStructureImpl<typename TypeAt<Is, TypeList>::Type>(offset, index), ...);
	}

	template<typename T>
	constexpr void _ParseDataStructureImpl(uint64_t& offset, size_t& index)
	{
		m_TypeOffset[index] = { offset, sizeof(T) };
		TypeToIndex<T>::Index = index;
		offset += m_ElementMaxCount * sizeof(T);
		index++;
	}

	static uint64_t GetHandleKey(uint64_t ChunkIndex, uint64_t DataIndex)
	{
		return (ChunkIndex << 32) | DataIndex;
	}

private:
	// 所有已经申请的内存
	std::vector<void*> m_AllChunks;

	// 已经用掉的内存
	std::vector<void*> m_UsedChunks;

	// 可用的内存块
	std::stack<void*> m_FreeChunks;

	// 当前块的数据索引
	uint64_t m_CurIndex{};

	// TypeHash -> [Offset, Length]
	std::vector<std::tuple<uint64_t, uint64_t>> m_TypeOffset;

	// 最大Element数量
	uint64_t m_ElementMaxCount{};

	// 分配出去的内存Handle
	std::unordered_map<uint64_t, ChunkHandle*> m_Handles;

	// 类型大小
	size_t m_ElementSize = 0;
};
#endif // FIXEDCHUNKCACHEFRIENDLYLINK_
