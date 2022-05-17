#pragma once
#include "MaterialObject.h"
#include "ShaderProgram.h"
#include "TextureManager.h"
#include <DirectXMath.h>

#include "CpuBuffer.h"
#include "ShaderParamCBuffer.h"
#include "ShaderParamMatrix.h"
#include "ShaderParamScalar.h"
#include "ShaderParamVector.h"

// MaterialObject存放的是
// 1.名称-int float vector
// 2.名称-texture key
// 简单类型序列化到json中
// Buffer Matrix是在运行时赋值
class Material
{
public:
	bool IsDirty();

	template<typename T>
	void SetVal(const std::string& ID, const T& val);

	template<typename T>
	void SetVector(const std::string& ID, const T& val);

	template<typename T>
	void SetMatrix(const std::string& ID, const T& val);

	void SetTexture(const std::string& ID, TextureRef val);

	template<typename T>
	void SetBuffer(const std::string& ID, const T& val);

	// 设置一些共享Buffer
	void SetBuffer(const std::string& ID, CpuBuffer* buffer);

	template<typename T>
	const T* GetVal(const std::string& ID);

	template<typename T>
	const T& GetVector(const std::string& ID);

	template<typename T>
	const T* GetMatrix(const std::string& ID);

	template<typename T>
	const T* GetBuffer(const std::string& ID);

	TextureRef GetTexture(const std::string& ID);

public:
	bool m_IsDirty{};
	MaterialSerialization::MaterialObject MaterialObject;
	std::shared_ptr<ShaderProgram> ShaderProgram;

	std::unordered_map<std::string, TextureRef> Textures{};
	std::unordered_map<std::string, CpuBuffer*> CBuffers{};

private:
	[[nodiscard]] bool CheckType(const std::string& ID, ParameterType Type) const;

};

inline bool Material::IsDirty()
{
	return m_IsDirty;
}

template <typename T>
void Material::SetVal(const std::string& ID, const T& val)
{
	if (!CheckType(ID, ParameterType::Scalar))
	{
		return;
	}
	ShaderParamScalar* scalar = static_cast<ShaderParamScalar*>(ShaderProgram->GetParam(ID));
	if (!scalar)
	{
		return;
	}
	ShaderParamCBuffer* cbuffer = scalar->BindBuffer;
	assert(cbuffer != nullptr);
	CpuBuffer* buffer = CBuffers[cbuffer->Name];
	assert(scalar->Size == sizeof(val));
	buffer->UpdateBuffer(&val, scalar->Offset, scalar->Size);
}

template <typename T>
void Material::SetVector(const std::string& ID, const T& val)
{
	if (!CheckType(ID, ParameterType::Vector))
	{
		return;
	}
	ShaderParamVector* vec = static_cast<ShaderParamVector*>(ShaderProgram->GetParam(ID));
	if (!vec)
	{
		return;
	}
	ShaderParamCBuffer* cbuffer = vec->BindBuffer;
	assert(cbuffer != nullptr);
	CpuBuffer* buffer = CBuffers[cbuffer->Name];
	assert(vec->Size == sizeof(val));
	buffer->UpdateBuffer(&val, vec->Offset, vec->Size);
}

template <typename T>
void Material::SetMatrix(const std::string& ID, const T& val)
{
	if (!CheckType(ID, ParameterType::Matrix))
	{
		return;
	}
	ShaderParamMatrix* matrix = static_cast<ShaderParamMatrix*>(ShaderProgram->GetParam(ID));
	if (!matrix)
	{
		return;
	}
	ShaderParamCBuffer* cbuffer = matrix->BindBuffer;
	assert(cbuffer != nullptr);
	CpuBuffer* buffer = CBuffers[cbuffer->Name];
	assert(matrix->Size == sizeof(val));
	buffer->UpdateBuffer(&val, matrix->Offset, matrix->Size);
}

inline void Material::SetTexture(const std::string& ID, TextureRef val)
{
	if (!CheckType(ID, ParameterType::Texture))
	{
		return;
	}
	Textures[ID] = val;
}

template <typename T>
void Material::SetBuffer(const std::string& ID, const T& val)
{
	if (!CheckType(ID, ParameterType::CBuffer))
	{
		return;
	}
}

inline void Material::SetBuffer(const std::string& ID, CpuBuffer* buffer)
{
	if (!CheckType(ID, ParameterType::CBuffer))
	{
		return;
	}
	if (CBuffers.contains(ID))
	{
		CpuBuffer::Destroy(CBuffers[ID]);
	}
	CBuffers[ID] = buffer;
}

template <typename T>
const T* Material::GetVal(const std::string& ID)
{
	assert(CBuffers.contains(ID));
	ShaderParamScalar* scalar = static_cast<ShaderParamScalar*>(ShaderProgram->GetParam(ID));
	if (!scalar)
	{
		return nullptr;
	}
	ShaderParamCBuffer* cbuffer = scalar->BindBuffer;
	assert(cbuffer != nullptr);
	CpuBuffer* buffer = CBuffers[cbuffer->Name];
	return reinterpret_cast<T*>(buffer->GetData(scalar->Offset));
}

template <typename T>
const T& Material::GetVector(const std::string& ID)
{
	ShaderParamVector* vec = static_cast<ShaderParamVector*>(ShaderProgram->GetParam(ID));
	if (!vec)
	{
		return nullptr;
	}
	ShaderParamCBuffer* cbuffer = vec->BindBuffer;
	assert(cbuffer != nullptr);
	CpuBuffer* buffer = CBuffers[cbuffer->Name];
	return reinterpret_cast<T*>(buffer->GetData(vec->Offset));
}

template <typename T>
const T* Material::GetMatrix(const std::string& ID)
{
	const ShaderParamMatrix* matrix = static_cast<ShaderParamMatrix*>(ShaderProgram->GetParam(ID));
	if (!matrix)
	{
		return nullptr;
	}
	const ShaderParamCBuffer* cbuffer = matrix->BindBuffer;
	assert(cbuffer != nullptr);
	const CpuBuffer* buffer = CBuffers[cbuffer->Name];
	return reinterpret_cast<T*>(buffer->GetData(matrix->Offset));
}

template <typename T>
const T* Material::GetBuffer(const std::string& ID)
{
	if (!CBuffers.contains(ID))
	{
		return nullptr;
	}
	;
	const CpuBuffer* buffer = CBuffers[ID];
	return reinterpret_cast<T*>(buffer->GetData());
}

inline TextureRef Material::GetTexture(const std::string& ID)
{
	assert(Textures.contains(ID));
	return Textures[ID];
}

inline bool Material::CheckType(const std::string& ID, ParameterType Type) const
{
	return ShaderProgram->CheckType(ID, Type);
}
