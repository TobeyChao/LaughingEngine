#include "PCH.h"
#include "ShaderProgram.h"

#include "ShaderParamCBuffer.h"
#include "ShaderParamMatrix.h"
#include "ShaderParamScalar.h"
#include "ShaderParamStruct.h"
#include "ShaderParamTexture.h"
#include "ShaderParamVariable.h"
#include "ShaderParamVector.h"

#include <iostream>
#include <wrl/client.h>

using namespace std;
using namespace Microsoft::WRL;

unordered_map<std::string, const char*> g_SemanticNameMapper =
{
	{ "POSITION", "POSITION"},
	{ "NORMAL", "NORMAL"},
	{ "TANGENT", "TANGENT"},
	{ "TEXCOORD", "TEXCOORD"},
	{ "SV_POSITION", "SV_POSITION"},
	{ "SV_TARGET", "SV_TARGET"},
};

void ShaderProgram::LoadFromMemory(const Utility::ByteArray vertexBytes, const Utility::ByteArray pixelBytes)
{
	m_VertexShader = vertexBytes;
	m_PixelShader = pixelBytes;
	ParseFromMemory(ShaderType::Vertex, vertexBytes);
	ParseFromMemory(ShaderType::Pixel, pixelBytes);
}

bool ShaderProgram::CheckType(const std::string& ID, ParameterType Type)
{
	if (!m_Parameters.contains(ID))
	{
		return false;
	}
	return m_Parameters[ID]->GetType() == Type;
}

ShaderParameter* ShaderProgram::GetParam(const std::string& ID)
{
	if (m_Parameters.contains(ID))
	{
		return m_Parameters[ID];
	}
	return nullptr;
}

void ShaderProgram::ParseFromMemory(ShaderType type, Utility::ByteArray bytes)
{
	ComPtr<ID3D12ShaderReflection> shader_reflection;
	D3DReflect(bytes->data(), bytes->size(), IID_PPV_ARGS(shader_reflection.GetAddressOf()));

	shader_reflection->GetRequiresFlags();

	// Shader描述
	D3D12_SHADER_DESC shader_desc = {};
	shader_reflection->GetDesc(&shader_desc);

	// 着色器版本
	UINT& Version = shader_desc.Version;
	UINT ProgramType = (Version & 0xFFFF0000) >> 16;
	UINT MajorVersion = (Version & 0x000000F0) >> 4;
	UINT MinorVersion = (Version & 0x0000000F);
	string VersionStr = ProgramType == 0 ? "ps" : "vs";
	string MajorVersionStr = "_" + to_string(MajorVersion);
	string MinorVersionStr = "_" + to_string(MinorVersion);
	Utility::Printf("Version: %s%s%s\n", VersionStr.c_str(), MajorVersionStr.c_str(), MinorVersionStr.c_str());

	// 着色器创建者的名称
	Utility::Printf("Creator: %s\n", shader_desc.Creator);

	// 着色器常量缓冲区的数量
	for (UINT i = 0; i < shader_desc.ConstantBuffers; ++i)
	{
		ID3D12ShaderReflectionConstantBuffer* sr_cb = shader_reflection->GetConstantBufferByIndex(i);
		D3D12_SHADER_BUFFER_DESC shader_buffer_desc = {};
		sr_cb->GetDesc(&shader_buffer_desc);
	}

	// 绑定到着色器的资源（纹理和缓冲区）的数量
	for (UINT i = 0; i < shader_desc.BoundResources; ++i)
	{
		D3D12_SHADER_INPUT_BIND_DESC shader_input_bind_desc = {};
		shader_reflection->GetResourceBindingDesc(i, &shader_input_bind_desc);

		OutputBoundResources(i, shader_input_bind_desc);

		switch (shader_input_bind_desc.Type)
		{
		case D3D_SIT_CBUFFER:
		{
			ID3D12ShaderReflectionConstantBuffer* sr_cb = shader_reflection->GetConstantBufferByName(shader_input_bind_desc.Name);
			D3D12_SHADER_BUFFER_DESC shader_buffer_desc = {};
			sr_cb->GetDesc(&shader_buffer_desc);

			ShaderParamCBuffer* cbuffer = new ShaderParamCBuffer();
			cbuffer->Index = shader_input_bind_desc.BindPoint;
			cbuffer->Count = shader_input_bind_desc.BindCount;
			cbuffer->Space = shader_input_bind_desc.Space;
			cbuffer->Size = shader_buffer_desc.Size;
			cbuffer->Variables = shader_buffer_desc.Variables;

			AddParameter(shader_buffer_desc.Name, cbuffer);

			for (UINT j = 0; j < shader_buffer_desc.Variables; ++j)
			{
				ID3D12ShaderReflectionVariable* variable = sr_cb->GetVariableByIndex(j);

				ID3D12ShaderReflectionType* variable_reflection = variable->GetType();

				D3D12_SHADER_VARIABLE_DESC shader_var_desc = {};
				variable->GetDesc(&shader_var_desc);

				D3D12_SHADER_TYPE_DESC shader_type_desc = {};
				variable_reflection->GetDesc(&shader_type_desc);

				ShaderParamVariable* shader_variable = nullptr;

				switch (shader_type_desc.Class)
				{
				case D3D_SVC_SCALAR:
					shader_variable = new ShaderParamScalar();
					break;
				case D3D_SVC_VECTOR:
					shader_variable = new ShaderParamVector();
					break;
				case D3D_SVC_MATRIX_ROWS:
				case D3D_SVC_MATRIX_COLUMNS:
					shader_variable = new ShaderParamMatrix();
					break;
				case D3D_SVC_OBJECT:
					break;
				case D3D_SVC_STRUCT:
					shader_variable = new ShaderParamStruct();
					break;
				case D3D_SVC_INTERFACE_CLASS:
					break;
				case D3D_SVC_INTERFACE_POINTER:
					break;
				case D3D_SVC_FORCE_DWORD:
					break;
				}

				if (shader_variable)
				{
					shader_variable->BindBuffer = cbuffer;
					shader_variable->StartOffset = shader_var_desc.StartOffset;
					shader_variable->Size = shader_var_desc.Size;
					shader_variable->Class = shader_type_desc.Class;
					shader_variable->Type = shader_type_desc.Type;
					shader_variable->Rows = shader_type_desc.Rows;
					shader_variable->Columns = shader_type_desc.Columns;
					shader_variable->Elements = shader_type_desc.Elements;
					shader_variable->Members = shader_type_desc.Members;
					shader_variable->Offset = shader_type_desc.Offset;
				}

				AddParameter(shader_var_desc.Name, shader_variable);

				OutputVariable(j, variable);
			}
			Utility::Printf("\n");
		}
		break;
		case D3D_SIT_TBUFFER:
			break;
		case D3D_SIT_TEXTURE:
		{
			ID3D12ShaderReflectionVariable* variable = shader_reflection->GetVariableByName(shader_input_bind_desc.Name);
			D3D12_SHADER_VARIABLE_DESC shader_var_desc = {};
			variable->GetDesc(&shader_var_desc);

			ID3D12ShaderReflectionType* reflectionType = variable->GetType();

			D3D12_SHADER_TYPE_DESC shader_type_desc = {};
			reflectionType->GetDesc(&shader_type_desc);

			ShaderParamTexture* tex = new ShaderParamTexture();
			tex->Space = shader_input_bind_desc.Space;
			tex->Count = shader_input_bind_desc.BindCount;
			tex->Index = shader_input_bind_desc.BindPoint;
			tex->Dimension = shader_input_bind_desc.Dimension;
			AddParameter(shader_input_bind_desc.Name, tex);
		}
		break;
		case D3D_SIT_SAMPLER:
			break;
		case D3D_SIT_UAV_RWTYPED:
			break;
		case D3D_SIT_STRUCTURED:
		{
			ID3D12ShaderReflectionConstantBuffer* sr_cb = shader_reflection->GetConstantBufferByName(shader_input_bind_desc.Name);
			D3D12_SHADER_BUFFER_DESC shader_buffer_desc = {};
			sr_cb->GetDesc(&shader_buffer_desc);

			ShaderParamCBuffer* cbuffer = new ShaderParamCBuffer();
			cbuffer->Index = shader_input_bind_desc.BindPoint;
			cbuffer->Count = shader_input_bind_desc.BindCount;
			cbuffer->Space = shader_input_bind_desc.Space;
			cbuffer->Size = shader_buffer_desc.Size;
			cbuffer->Variables = shader_buffer_desc.Variables;

			AddParameter(shader_buffer_desc.Name, cbuffer);

			for (UINT j = 0; j < shader_buffer_desc.Variables; ++j)
			{
				ID3D12ShaderReflectionVariable* variable = sr_cb->GetVariableByIndex(j);

				ID3D12ShaderReflectionType* variable_reflection = variable->GetType();

				D3D12_SHADER_VARIABLE_DESC shader_var_desc = {};
				variable->GetDesc(&shader_var_desc);

				D3D12_SHADER_TYPE_DESC shader_type_desc = {};
				variable_reflection->GetDesc(&shader_type_desc);

				ShaderParamVariable* shader_variable = nullptr;

				switch (shader_type_desc.Class)
				{
				case D3D_SVC_SCALAR:
					shader_variable = new ShaderParamScalar();
					break;
				case D3D_SVC_VECTOR:
					shader_variable = new ShaderParamVector();
					break;
				case D3D_SVC_MATRIX_ROWS:
				case D3D_SVC_MATRIX_COLUMNS:
					shader_variable = new ShaderParamMatrix();
					break;
				case D3D_SVC_OBJECT:
					break;
				case D3D_SVC_STRUCT:
					shader_variable = new ShaderParamStruct();
					break;
				case D3D_SVC_INTERFACE_CLASS:
					break;
				case D3D_SVC_INTERFACE_POINTER:
					break;
				case D3D_SVC_FORCE_DWORD:
					break;
				default:
					shader_variable = nullptr;
					break;
				}

				if (shader_variable)
				{
					shader_variable->BindBuffer = cbuffer;
					shader_variable->StartOffset = shader_var_desc.StartOffset;
					shader_variable->Size = shader_var_desc.Size;
					shader_variable->Class = shader_type_desc.Class;
					shader_variable->Type = shader_type_desc.Type;
					shader_variable->Rows = shader_type_desc.Rows;
					shader_variable->Columns = shader_type_desc.Columns;
					shader_variable->Elements = shader_type_desc.Elements;
					shader_variable->Members = shader_type_desc.Members;
					shader_variable->Offset = shader_type_desc.Offset;
				}

				AddParameter(shader_var_desc.Name, shader_variable);

				OutputVariable(j, variable);
			}
			Utility::Printf("\n");
		}
		break;
		case D3D_SIT_UAV_RWSTRUCTURED:
			break;
		case D3D_SIT_BYTEADDRESS:
			break;
		case D3D_SIT_UAV_RWBYTEADDRESS:
			break;
		case D3D_SIT_UAV_APPEND_STRUCTURED:
			break;
		case D3D_SIT_UAV_CONSUME_STRUCTURED:
			break;
		case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
			break;
		case D3D_SIT_RTACCELERATIONSTRUCTURE:
			break;
		case D3D_SIT_UAV_FEEDBACKTEXTURE:
			break;
		}
	}

	// 输入签名中的参数
	UINT offset = 0;
	for (UINT i = 0; i < shader_desc.InputParameters; ++i)
	{
		D3D12_SIGNATURE_PARAMETER_DESC signature_parameter_desc = {};
		shader_reflection->GetInputParameterDesc(i, &signature_parameter_desc);
		Utility::Printf("IN D3D12_SIGNATURE_PARAMETER_DESC At %d SemanticName:%s SemanticIndex:%d Mask:%d\n",
			i, signature_parameter_desc.SemanticName, signature_parameter_desc.SemanticIndex, (UINT)signature_parameter_desc.Mask);
		if (type == ShaderType::Vertex)
		{
			switch ((UINT)signature_parameter_desc.Mask)
			{
			case 0b0001:
			{
				AddInputElement(signature_parameter_desc.SemanticName, DXGI_FORMAT_R32_FLOAT, offset);
				offset += 4;
			}break;
			case 0b0011:
			{
				AddInputElement(signature_parameter_desc.SemanticName, DXGI_FORMAT_R32G32_FLOAT, offset);
				offset += 8;
			}break;
			case 0b0111:
			{
				AddInputElement(signature_parameter_desc.SemanticName, DXGI_FORMAT_R32G32B32_FLOAT, offset);
				offset += 12;
			}break;
			case 0b1111:
			{
				AddInputElement(signature_parameter_desc.SemanticName, DXGI_FORMAT_R32G32B32A32_FLOAT, offset);
				offset += 16;
			}break;
			default:
				break;
			}
		}
	}

	// 输出签名中的参数
	for (UINT i = 0; i < shader_desc.OutputParameters; ++i)
	{
		D3D12_SIGNATURE_PARAMETER_DESC signature_parameter_desc = {};
		shader_reflection->GetOutputParameterDesc(i, &signature_parameter_desc);
		Utility::Printf("OUT D3D12_SIGNATURE_PARAMETER_DESC At Index %d : %s - %d - %d\n", i, signature_parameter_desc.SemanticName, signature_parameter_desc.SemanticIndex, signature_parameter_desc.Register);
	}

	//{
	//	// 已编译着色器中的中间语言指令数
	//	cout << shader_desc.InstructionCount << endl;

	//	// 已编译着色器中临时寄存器的数量
	//	cout << shader_desc.TempRegisterCount << endl;

	//	// 使用的临时数组的数量
	//	cout << shader_desc.TempArrayCount << endl;

	//	// 常量定义的数量
	//	cout << shader_desc.DefCount << endl;

	//	// 声明数（输入 + 输出）
	//	cout << shader_desc.DclCount << endl;

	//	// 未分类纹理指令的数量
	//	cout << shader_desc.TextureNormalInstructions << endl;

	//	// 纹理加载指令数
	//	cout << shader_desc.TextureLoadInstructions << endl;

	//	// 纹理比较指令数
	//	cout << shader_desc.TextureCompInstructions << endl;

	//	// 纹理偏差指令数
	//	cout << shader_desc.TextureBiasInstructions << endl;

	//	// 纹理渐变指令的数量
	//	cout << shader_desc.TextureGradientInstructions << endl;

	//	// 使用的浮点算术指令数
	//	cout << shader_desc.FloatInstructionCount << endl;

	//	// 使用的有符号整数算术指令数
	//	cout << shader_desc.IntInstructionCount << endl;

	//	// 使用的无符号整数算术指令数
	//	cout << shader_desc.UintInstructionCount << endl;

	//	// 使用的静态流控制指令数
	//	cout << shader_desc.StaticFlowControlCount << endl;

	//	// 使用的动态流控制指令数
	//	cout << shader_desc.DynamicFlowControlCount << endl;

	//	// 使用的宏指令数
	//	cout << shader_desc.MacroInstructionCount << endl;

	//	// 使用的数组指令数
	//	cout << shader_desc.ArrayInstructionCount << endl;

	//	// 使用的剪切指令数
	//	cout << shader_desc.CutInstructionCount << endl;

	//	// 使用的发射指令数
	//	cout << shader_desc.EmitInstructionCount << endl;

	//	// 表示几何着色器输出拓扑的D3D_PRIMITIVE_TOPOLOGY类型值
	//	cout << shader_desc.GSOutputTopology << endl;

	//	// 几何着色器最大输出顶点数
	//	cout << shader_desc.GSMaxOutputVertexCount << endl;

	//	// D3D_PRIMITIVE类型的值，表示几何着色器或外壳着色器的输入图元
	//	cout << shader_desc.InputPrimitive << endl;

	//	// 补丁常量签名中的参数数量
	//	cout << shader_desc.PatchConstantParameters << endl;

	//	// 几何着色器实例的数量
	//	cout << shader_desc.cGSInstanceCount << endl;

	//	// 外壳着色器和域着色器中的控制点数
	//	cout << shader_desc.cControlPoints << endl;

	//	// 表示镶嵌器输出基元类型的D3D_TESSELLATOR_OUTPUT_PRIMITIVE类型值
	//	cout << shader_desc.HSOutputPrimitive << endl;

	//	// 表示镶嵌器分区模式的D3D_TESSELLATOR_PARTITIONING类型的值
	//	cout << shader_desc.HSPartitioning << endl;

	//	// 表示曲面细分器域的D3D_TESSELLATOR_DOMAIN类型值
	//	cout << shader_desc.TessellatorDomain << endl;

	//	// 计算着色器中的屏障指令数
	//	cout << shader_desc.cBarrierInstructions << endl;

	//	// 计算着色器中的互锁指令数
	//	cout << shader_desc.cInterlockedInstructions << endl;

	//	// 计算着色器中的纹理写入次数
	//	cout << shader_desc.cTextureStoreInstructions << endl;
	//}
}

void ShaderProgram::AddParameter(const string& Name, ShaderParameter* parameter)
{
	parameter->Name = Name;
	m_Parameters[Name] = parameter;
	switch (parameter->GetType())
	{
	case ParameterType::Scalar: break;
	case ParameterType::Vector: break;
	case ParameterType::Matrix: break;
	case ParameterType::Struct: break;
	case ParameterType::Texture:
		m_Textures.push_back(parameter);
		break;
	case ParameterType::CBuffer: break;
	case ParameterType::TBuffer: break;
	case ParameterType::Count: break;
	default:;
	}
}

void ShaderProgram::AddInputElement(const std::string& SemanticName, DXGI_FORMAT Format, UINT AlignedByteOffset)
{
	char* str = new char[SemanticName.size() + 1];
	strcpy_s(str, SemanticName.size() + 1, SemanticName.c_str());
	m_InputLayout.push_back({ str, 0, Format, AlignedByteOffset, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
}

void ShaderProgram::OutputBoundResources(UINT i, const D3D12_SHADER_INPUT_BIND_DESC& desc)
{
	Utility::Printf("Bound Resources At Index %d Name:%s Type:%d BindPoint:%d BindCount:%d Dimension:%d Space:%d\n",
		i, desc.Name, desc.Type, desc.BindPoint, desc.BindCount, desc.Dimension, desc.Space);
}

void ShaderProgram::OutputVariable(UINT j, ID3D12ShaderReflectionVariable* variable)
{
	ID3D12ShaderReflectionType* type = variable->GetType();

	D3D12_SHADER_VARIABLE_DESC shader_var_desc = {};
	variable->GetDesc(&shader_var_desc);

	D3D12_SHADER_TYPE_DESC shader_type_desc = {};
	type->GetDesc(&shader_type_desc);
	Utility::Printf("Var At %d Name:%s Offset:%d Size:%d TypeName:%s Members:%d Elements:%d Offset:%d\n",
		j, shader_var_desc.Name, shader_var_desc.StartOffset, shader_var_desc.Size, shader_type_desc.Name, shader_type_desc.Members, shader_type_desc.Elements, shader_type_desc.Offset);
	OutputType(type);
}

void ShaderProgram::OutputType(ID3D12ShaderReflectionType* type)
{
	D3D12_SHADER_TYPE_DESC shader_type_desc = {};
	type->GetDesc(&shader_type_desc);

	for (UINT x = 0; x < shader_type_desc.Members; ++x)
	{
		ID3D12ShaderReflectionType* member_type = type->GetMemberTypeByIndex(x);
		D3D12_SHADER_TYPE_DESC member_type_desc = {};
		member_type->GetDesc(&member_type_desc);
		Utility::Printf("Member At %d Name:%s TypeName:%s Members:%d Elements:%d Offset:%d\n",
			x, type->GetMemberTypeName(x), member_type_desc.Name, shader_type_desc.Members, shader_type_desc.Elements, shader_type_desc.Offset);

		OutputType(member_type);
	}
}
