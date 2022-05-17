#pragma once
#include "PCH.h"
#include "Texture.h"

class ManagedTexture;
class TextureRef;

namespace TextureManager
{
	TextureRef LoadFromFile(const std::wstring& FileName);
}

class TextureRef
{
public:
	TextureRef(const TextureRef& ref);
	TextureRef(ManagedTexture* tex = nullptr);
	~TextureRef();

	TextureRef& operator= (std::nullptr_t);
	TextureRef& operator= (const TextureRef& rhs);

	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const;

	[[nodiscard]] const Texture* Get() const;

	[[nodiscard]] uint32_t GetIndexInHeap() const;

	void SetIndexInHeap(uint32_t IndexInHeap);

private:
	ManagedTexture* m_pTexture;
};