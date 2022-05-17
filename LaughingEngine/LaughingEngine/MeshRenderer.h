#pragma once
#include "Material.h"
#include "MeshGeometry.h"

enum class RenderLayer : uint8_t
{
	Background,
	Geometry,
	AlphaTest,
	GeometryLast,
	Transparent,
	Overlay,
	Count
};

struct MeshRenderer
{
	MeshGeometry* Mesh = nullptr;
	SubmeshGeometry* SubMesh = nullptr;
	Material* Material = nullptr;
	RenderLayer RenderLayer;
};