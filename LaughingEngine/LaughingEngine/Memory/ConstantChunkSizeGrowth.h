#pragma once
#include <cstdint>

class ConstantChunkSizeGrowth
{
public:
	ConstantChunkSizeGrowth(uint32_t chunkSizeInBytes = 0x4000)
		:
		m_ChunkSizeInBytes(chunkSizeInBytes)
	{}

protected:
	uint32_t GetBytesToPreAllocate() const
	{
		return m_ChunkSizeInBytes;
	}

	uint32_t GetBytesToGrow() const
	{
		return m_ChunkSizeInBytes;
	}

private:
	uint32_t m_ChunkSizeInBytes;
};