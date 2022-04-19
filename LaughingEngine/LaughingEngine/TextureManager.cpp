#include "PCH.h"
#include "TextureManager.h"
#include <map>

namespace TextureManager
{
	void DestroyTexture(const std::wstring& key);
}

class ManagedTexture : public Texture
{
	friend class TextureRef;
public:
	ManagedTexture(const std::wstring& FileName);
	~ManagedTexture();
	void CreateFromMemory(Utility::ByteArray Data, bool IssRGB);
	void Unload();

private:
	std::wstring m_FileName;
	size_t m_ReferCount;
};

ManagedTexture::ManagedTexture(const std::wstring& FileName)
	:
	m_FileName(FileName),
	m_ReferCount(0)
{
}

ManagedTexture::~ManagedTexture()
{
	Destroy();
}

void ManagedTexture::CreateFromMemory(Utility::ByteArray Data, bool IssRGB)
{
	Texture::CreateDDSFromMemory(Data->data(), Data->size(), IssRGB);
}

void ManagedTexture::Unload()
{
	TextureManager::DestroyTexture(m_FileName);
}

TextureRef::TextureRef(ManagedTexture* Texture)
	:
	m_pTexture(Texture)
{
	if (Texture)
	{
		Texture->m_ReferCount++;
	}
}

TextureRef::TextureRef(const TextureRef& Ref)
	:
	m_pTexture(Ref.m_pTexture)
{
	if (m_pTexture)
	{
		m_pTexture->m_ReferCount++;
	}
}

TextureRef::~TextureRef()
{
	if (m_pTexture && --m_pTexture->m_ReferCount == 0)
	{
		TextureManager::DestroyTexture(m_pTexture->m_FileName);
	}
}

void TextureRef::operator=(std::nullptr_t)
{
	if (m_pTexture)
	{
		m_pTexture->m_ReferCount--;
		m_pTexture = nullptr;
	}
}

void TextureRef::operator=(const TextureRef& rhs)
{
	if (m_pTexture)
		--m_pTexture->m_ReferCount;

	m_pTexture = rhs.m_pTexture;

	if (m_pTexture != nullptr)
		++m_pTexture->m_ReferCount;
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureRef::GetSRV() const
{
	if (m_pTexture)
	{
		return m_pTexture->GetSRV();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE handle;
	handle.ptr = -1;
	return handle;
}

const Texture* TextureRef::Get() const
{
	return m_pTexture;
}

namespace TextureManager
{
	std::map<std::wstring, std::unique_ptr<ManagedTexture>> s_TextureCache;

	ManagedTexture* FindOrLoadTexture(const std::wstring FileName)
	{
		ManagedTexture* ret = nullptr;

		// 1.先在Cache中寻找
		auto iter = s_TextureCache.find(FileName);
		if (iter != s_TextureCache.end())
		{
			ret = iter->second.get();
			return ret;
		}
		else
		{
			ret = new ManagedTexture(FileName);
			s_TextureCache[FileName].reset(ret);
		}
		// 没有找到需要加载
		auto Data = Utility::LoadFileSync(FileName);
		ret->CreateFromMemory(Data, false);
		return ret;
	}

	TextureRef LoadDDSFromFile(const std::wstring FileName)
	{
		return FindOrLoadTexture(FileName);
	}

	void DestroyTexture(const std::wstring& key)
	{
		auto iter = s_TextureCache.find(key);
		if (iter != s_TextureCache.end())
		{
			s_TextureCache.erase(iter);
		}
	}
}