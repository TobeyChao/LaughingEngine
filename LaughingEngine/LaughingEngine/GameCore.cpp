#include "GameCore.h"
#include "GraphicsCore.h"
#include "GameTimer.h"
#include "Display.h"

namespace Game
{
	using namespace Graphics;

	void InitializeApplication(IGameApp& game)
	{
		Graphics::Initialize();
		GameTimer::Initialize();
		game.Initialize();
	}

	void UpdateApplication(IGameApp& game)
	{
		Display::Update();

		game.Update();
		game.Draw();

		Display::Present();
	}

	void TerminateApplication(IGameApp& game)
	{
		game.Shutdown();
		Graphics::Shutdown();
	}

	HWND g_hWnd = nullptr;

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	int RunApplication(IGameApp& app, const wchar_t* className, HINSTANCE hInst, int nCmdShow)
	{
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = LoadIcon(hInst, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = className;
		wcex.hIconSm = LoadIcon(hInst, IDI_APPLICATION);

		if (!RegisterClassEx(&wcex))
		{
			MessageBox(0, L"RegisterClassEx Failed.", 0, 0);
			return 1;
		}

		RECT rc = { 0, 0, static_cast<LONG>(Graphics::g_DisplayWidth), static_cast<LONG>(Graphics::g_DisplayHeight) };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

		g_hWnd = CreateWindow(className, className, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, nullptr);

		InitializeApplication(app);

		ShowWindow(g_hWnd, nCmdShow/*SW_SHOWDEFAULT*/);

		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}
			UpdateApplication(app);
		}

		TerminateApplication(app);

		DestroyWindow(g_hWnd);
		UnregisterClass(wcex.lpszClassName, wcex.hInstance);
		g_hWnd = nullptr;

		return 0;
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_SIZE:
			Display::Resize((UINT)(UINT64)lParam & 0xFFFF, (UINT)(UINT64)lParam >> 16);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		return 0;
	}
}
