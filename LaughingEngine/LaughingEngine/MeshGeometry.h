#pragma once
#include "PCH.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>

using DirectX::BoundingBox;
using Microsoft::WRL::ComPtr;

// 子网格需要有单独的材质
struct SubmeshGeometry
{
	// 索引数量
	UINT IndexCount = 0;
	// 索引Index
	UINT StartIndexLocation = 0;
	// 顶点Index
	INT BaseVertexLocation = 0;
};

// 网格包含多个子网格
struct MeshGeometry
{
	std::wstring Name;
	// 顶点Buffer 内存
	ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	// 索引Buffer 内存
	ComPtr<ID3DBlob> IndexBufferCPU = nullptr;
	// 顶点Buffer
	VertexBuffer VertexBufferGPU;
	// 索引Buffer
	IndexBuffer IndexBufferGPU;
	// 子Mesh
	std::unordered_map<std::wstring, SubmeshGeometry> DrawArgs;
	// 图元类型
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	// 包围盒
	BoundingBox Bounds;
};