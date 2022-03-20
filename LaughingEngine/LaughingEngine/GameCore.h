#pragma once

#include "PCH.h"

namespace Game
{
	extern HWND g_hWnd;

	class IGameApp
	{
	public:
		virtual void Initialize() = 0;
		virtual void Update() = 0;
		virtual void Draw() = 0;
		virtual void Shutdown() = 0;
	};
	
	int RunApplication(IGameApp& app, const wchar_t* className, HINSTANCE hInst, int nCmdShow);
}