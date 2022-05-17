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

	// Shader����
	D3D12_SHADER_DESC shader_desc = {};
	shader_reflection->GetDesc(&shader_desc);

	// ��ɫ���汾
	UINT& Version = shader_desc.Version;
	UINT ProgramType = (Version & 0xFFFF0000) >> 16;
	UINT MajorVersion = (Version & 0x000000F0) >> 4;
	UINT MinorVersion = (Version & 0x0000000F);
	string VersionStr = ProgramType == 0 ? "ps" : "vs";
	string MajorVersionStr = "_" + to_string(MajorVersion);
	string MinorVersionStr = "_" + to_string(MinorVersion);
	Utility::Printf("Version: %s%s%s\n", VersionStr.c_str(), MajorVersionStr.c_str(), MinorVersionStr.c_str());

	// ��ɫ�������ߵ�����
	Utility::Printf("Creator: %s\n", shader_desc.Creator);

	// ��ɫ������������������
	for (UINT i = 0; i < shader_desc.ConstantBuffers; ++i)
	{
		ID3D12ShaderReflectionConstantBuffer* sr_cb = shader_reflection->GetConstantBufferByIndex(i);
		D3D12_SHADER_BUFFER_DESC shader_buffer_desc = {};
		sr_cb->GetDesc(&shader_buffer_desc);
	}

	// �󶨵���ɫ������Դ������ͻ�������������
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

	// ����ǩ���еĲ���
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

	// ���ǩ���еĲ���
	for (UINT i = 0; i < shader_desc.OutputParameters; ++i)
	{
		D3D12_SIGNATURE_PARAMETER_DESC signature_parameter_desc = {};
		shader_reflection->GetOutputParameterDesc(i, &signature_parameter_desc);
		Utility::Printf("OUT D3D12_SIGNATURE_PARAMETER_DESC At Index %d : %s - %d - %d\n", i, signature_parameter_desc.SemanticName, signature_parameter_desc.SemanticIndex, signature_parameter_desc.Register);
	}

	//{
	//	// �ѱ�����ɫ���е��м�����ָ����
	//	cout << shader_desc.InstructionCount << endl;

	//	// �ѱ�����ɫ������ʱ�Ĵ���������
	//	cout << shader_desc.TempRegisterCount << endl;

	//	// ʹ�õ���ʱ���������
	//	cout << shader_desc.TempArrayCount << endl;

	//	// �������������
	//	cout << shader_desc.DefCount << endl;

	//	// ������������ + �����
	//	cout << shader_desc.DclCount << endl;

	//	// δ��������ָ�������
	//	cout << shader_desc.TextureNormalInstructions << endl;

	//	// �������ָ����
	//	cout << shader_desc.TextureLoadInstructions << endl;

	//	// ����Ƚ�ָ����
	//	cout << shader_desc.TextureCompInstructions << endl;

	//	// ����ƫ��ָ����
	//	cout << shader_desc.TextureBiasInstructions << endl;

	//	// ������ָ�������
	//	cout << shader_desc.TextureGradientInstructions << endl;

	//	// ʹ�õĸ�������ָ����
	//	cout << shader_desc.FloatInstructionCount << endl;

	//	// ʹ�õ��з�����������ָ����
	//	cout << shader_desc.IntInstructionCount << endl;

	//	// ʹ�õ��޷�����������ָ����
	//	cout << shader_desc.UintInstructionCount << endl;

	//	// ʹ�õľ�̬������ָ����
	//	cout << shader_desc.StaticFlowControlCount << endl;

	//	// ʹ�õĶ�̬������ָ����
	//	cout << shader_desc.DynamicFlowControlCount << endl;

	//	// ʹ�õĺ�ָ����
	//	cout << shader_desc.MacroInstructionCount << endl;

	//	// ʹ�õ�����ָ����
	//	cout << shader_desc.ArrayInstructionCount << endl;

	//	// ʹ�õļ���ָ����
	//	cout << shader_desc.CutInstructionCount << endl;

	//	// ʹ�õķ���ָ����
	//	cout << shader_desc.EmitInstructionCount << endl;

	//	// ��ʾ������ɫ��������˵�D3D_PRIMITIVE_TOPOLOGY����ֵ
	//	cout << shader_desc.GSOutputTopology << endl;

	//	// ������ɫ��������������
	//	cout << shader_desc.GSMaxOutputVertexCount << endl;

	//	// D3D_PRIMITIVE���͵�ֵ����ʾ������ɫ���������ɫ��������ͼԪ
	//	cout << shader_desc.InputPrimitive << endl;

	//	// ��������ǩ���еĲ�������
	//	cout << shader_desc.PatchConstantParameters << endl;

	//	// ������ɫ��ʵ��������
	//	cout << shader_desc.cGSInstanceCount << endl;

	//	// �����ɫ��������ɫ���еĿ��Ƶ���
	//	cout << shader_desc.cControlPoints << endl;

	//	// ��ʾ��Ƕ�������Ԫ���͵�D3D_TESSELLATOR_OUTPUT_PRIMITIVE����ֵ
	//	cout << shader_desc.HSOutputPrimitive << endl;

	//	// ��ʾ��Ƕ������ģʽ��D3D_TESSELLATOR_PARTITIONING���͵�ֵ
	//	cout << shader_desc.HSPartitioning << endl;

	//	// ��ʾ����ϸ�������D3D_TESSELLATOR_DOMAIN����ֵ
	//	cout << shader_desc.TessellatorDomain << endl;

	//	// ������ɫ���е�����ָ����
	//	cout << shader_desc.cBarrierInstructions << endl;

	//	// ������ɫ���еĻ���ָ����
	//	cout << shader_desc.cInterlockedInstructions << endl;

	//	// ������ɫ���е�����д�����
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
