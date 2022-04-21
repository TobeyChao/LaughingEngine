#pragma once
#include "PCH.h"
#include "Texture.h"

class ManagedTexture;
class TextureRef;

namespace TextureManager
{
	TextureRef LoadFromFile(const std::wstring FileName);
}

class TextureRef
{
public:
	TextureRef(const TextureRef& ref);
	TextureRef(ManagedTexture* tex = nullptr);
	~TextureRef();

	void operator= (std::nullptr_t);
	void operator= (const TextureRef& rhs);

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const;

	const Texture* Get(void) const;

private:
	ManagedTexture* m_pTexture;
};