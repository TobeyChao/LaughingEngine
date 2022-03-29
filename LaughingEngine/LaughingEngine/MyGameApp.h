#pragma once
#include "GameCore.h"
#include "Camera.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "GpuBuffer.h"
#include <DirectXMath.h>
#include <unordered_map>

using DirectX::XMConvertToRadians;
using namespace Game;

class MyGameApp : public Game::IGameApp
{
public:
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void Shutdown() override;
private:
	CD3DX12_VIEWPORT m_MainViewport;
	CD3DX12_RECT m_MainScissor;

	std::unordered_map<std::string, std::unique_ptr<Camera>> m_Cameras;
	float m_Yaw = 0;
	float m_Pitch = XMConvertToRadians(15);
	float m_CamMoveSpeed = 20.f;
	POINT m_LastMousePos;

	GraphicsPiplelineState* m_DefaultPSO = nullptr;
	RootSignature m_DefaultRS;
	VertexBuffer m_BoxVertexBuffer;
	IndexBuffer m_BoxIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_BoxIndexBufferView;
	D3D12_VERTEX_BUFFER_VIEW m_BoxVertexBufferView;
	ByteAddressBuffer m_ObjPerObject;
};