#pragma once
#include "PCH.h"

class CpuBuffer
{
public:
	// ����
	static CpuBuffer* Create(uint64_t Count, uint64_t Size)
	{
		auto* buffer = new CpuBuffer();
		buffer->m_ElementCount = Count;
		buffer->m_ElementSize = Size;
		buffer->m_BufferSize = Count * Size;
		buffer->Data = malloc(buffer->m_BufferSize);
		return buffer;
	}

	// ����
	static void Destroy(CpuBuffer* buffer)
	{
		if (buffer->Data)
		{
			free(buffer->Data);
			buffer->Data = nullptr;
		}
	}

	// ����Buffer
	void UpdateBuffer(const void* InitData, size_t Offset, size_t Size) const
	{
		assert(Offset + Size <= m_BufferSize);
		memcpy(static_cast<uint8_t*>(Data) + Offset, InitData, Size);
	}

	// Buffer�Ĵ�С
	[[nodiscard]] size_t BufferSize() const
	{
		return m_BufferSize;
	}

	// ����Ĵ�С
	[[nodiscard]] uint64_t ElementCount() const
	{
		return m_ElementCount;
	}

	// ����
	[[nodiscard]] uint64_t ElementSize() const
	{
		return m_ElementSize;
	}

	[[nodiscard]] void* GetData(int Offset = 0) const
	{
		return static_cast<uint8_t*>(Data) + Offset;
	}

private:
	void* Data = nullptr;
	size_t m_BufferSize{};
	uint64_t m_ElementCount{};
	uint64_t m_ElementSize{};
};