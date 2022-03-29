#pragma once
#include "GameCore.h"
#include "Camera.h"
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

	std::unordered_map<std::string, std::unique_ptr<Camera>> mCameras;
	float mYaw = 0;
	float mPitch = XMConvertToRadians(15);
	float mCamMoveSpeed = 20.f;
	POINT mLastMousePos;


};