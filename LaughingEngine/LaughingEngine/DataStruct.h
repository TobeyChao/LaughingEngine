#pragma once
#include <DirectXMath.h>

using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMFLOAT4X4;

constexpr auto MaxLights = 16;

struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT2 TexCoord;
	XMFLOAT3 Normal;
	XMFLOAT4 Color;
};

struct Light
{
	XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
	float FalloffStart = 1.0f;					// point/spot light only
	XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };	// directional/spot light only
	float FalloffEnd = 10.0f;					// point/spot light only
	XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };	// point/spot light only
	float SpotPower = 64.0f;					// spot light only
};

struct PassConstants
{
	XMFLOAT4X4 View;
	XMFLOAT4X4 InvView;
	XMFLOAT4X4 Proj;
	XMFLOAT4X4 InvProj;
	XMFLOAT4X4 ViewProj;
	XMFLOAT4X4 InvViewProj;
	XMFLOAT4X4 ShadowTransform;
	XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	float cbPerObjectPad1 = 0.0f;
	XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;

	XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

	// Indices [0, NUM_DIR_LIGHTS) are directional lights;
	// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
	// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
	// are spot lights for a maximum of MaxLights per object.
	Light Lights[MaxLights];
};

struct ObjectConstants
{
	XMFLOAT4X4 World;
};

__declspec(align(16)) struct SkyboxVSCB
{
	XMFLOAT4X4 ProjInverse;
	XMFLOAT4X4 ViewInverse;
};


__declspec(align(16)) struct SkyboxPSCB
{
	float TextureLevel;
};