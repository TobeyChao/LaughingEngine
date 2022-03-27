#pragma once

#include "PCH.h"
#include "Texture.h"

class ManagedTexture;
class TextureRef;

namespace TextureManager
{
	TextureRef LoadDDSFromFile(const std::wstring FileName);
}

class TextureRef
{
public:
	TextureRef(ManagedTexture* Texture);
	TextureRef(const TextureRef& Ref);
	~TextureRef();

	void operator= (std::nullptr_t);
	void operator= (TextureRef& rhs);

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const;

	const Texture* Get(void) const;

private:
	ManagedTexture* m_pTexture;
};