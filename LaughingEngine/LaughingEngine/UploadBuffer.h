#pragma once
#include "GpuResource.h"

class UploadBuffer : public GpuResource
{
public:
	~UploadBuffer() override
	{
	}

	void Create(const std::wstring& Name, size_t BufferSize);

	void* Map();
	void Unmap(size_t Begin = 0, size_t End = -1);

	size_t GetBufferSize() const { return m_BufferSize; }

protected:
	size_t m_BufferSize = 0;
};