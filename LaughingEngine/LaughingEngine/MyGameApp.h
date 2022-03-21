#pragma once
#include "GameCore.h"

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
};