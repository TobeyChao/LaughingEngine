#pragma once
#include "PCH.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>

using DirectX::BoundingBox;
using Microsoft::WRL::ComPtr;

// ��������Ҫ�е����Ĳ���
struct SubmeshGeometry
{
	// ��������
	UINT IndexCount = 0;
	// ����Index
	UINT StartIndexLocation = 0;
	// ����Index
	INT BaseVertexLocation = 0;
};

// ����������������
struct MeshGeometry
{
	std::wstring Name;
	// ����Buffer �ڴ�
	ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	// ����Buffer �ڴ�
	ComPtr<ID3DBlob> IndexBufferCPU = nullptr;
	// ����Buffer
	VertexBuffer VertexBufferGPU;
	// ����Buffer
	IndexBuffer IndexBufferGPU;
	// ��Mesh
	std::unordered_map<std::wstring, SubmeshGeometry> DrawArgs;
	// ͼԪ����
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	// ��Χ��
	BoundingBox Bounds;
};