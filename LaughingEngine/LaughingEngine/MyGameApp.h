#pragma once
#include "GameCore.h"
#include "Camera.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "GpuBuffer.h"
#include "CommandContext.h"
#include "DataStruct.h"
#include "TextureManager.h"
#include "DescriptorHeap.h"
#include <DirectXMath.h>
#include <unordered_map>

using DirectX::XMConvertToRadians;
using namespace Game;

class MyGameApp : public Game::IGameApp
{
public:
	MyGameApp()
		:
		m_DefaultPSO(L"MyGameApp::m_DefaultPSO"),
		m_SkyPSO(L"MyGameApp::m_SkyPSO"),
		m_Angle(0.0f)
	{}
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void Shutdown() override;

private:
	void DrawBox(GraphicsContext& Context, const D3D12_VIEWPORT& Viewport, const D3D12_RECT& Scissor);
	void DrawSkybox(GraphicsContext& Context, const D3D12_VIEWPORT& Viewport, const D3D12_RECT& Scissor);

private:
	CD3DX12_VIEWPORT m_MainViewport;
	CD3DX12_RECT m_MainScissor;

	std::unordered_map<std::string, std::unique_ptr<Camera>> m_Cameras;

	GraphicsPiplelineState m_DefaultPSO;
	GraphicsPiplelineState m_SkyPSO;
	RootSignature m_DefaultRS;

	VertexBuffer m_BoxVertexBuffer;
	IndexBuffer m_BoxIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_BoxIndexBufferView;
	D3D12_VERTEX_BUFFER_VIEW m_BoxVertexBufferView;

	// 正方体旋转的角度
	float m_Angle;

	// 直射光方向控制变量
	float m_Theta = 0.0f;

	// Constants
	PassConstants m_MainPassCB;
	ObjectConstants m_ObjCB;
	SkyboxVSCB m_SkyboxVSCB;
	SkyboxPSCB m_SkyboxPSCB;

	// Textures
	std::vector<TextureRef> m_TextureReferences;

	// Texture的描述符堆
	DescriptorHeap m_TextureHeap;
};