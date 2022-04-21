#include "PCH.h"
#include "GameCore.h"
#include "GraphicsCore.h"
#include "GameTimer.h"
#include "InputSystem.h"
#include "Display.h"
#include "COMInitializer.h"

namespace Game
{
	using namespace Graphics;

	const std::wstring m_MainWndCaption = L"MainWindow";

	COMInitializer* g_COMInitializer = nullptr;

	void CalculateFrameStats()
	{
		// Code computes the average frames per second, and also the 
		// average time it takes to render one frame.  These stats 
		// are appended to the window caption bar.

		static int frameCnt = 0;
		static float timeElapsed = 0.0f;

		frameCnt++;

		// Compute averages over one second period.
		if ((GameTimer::TotalTime() - timeElapsed) >= 1.0f)
		{
			float fps = (float)frameCnt; // fps = frameCnt / 1
			float mspf = 1000.0f / fps;

			std::wstring fpsStr = std::to_wstring(fps);
			std::wstring mspfStr = std::to_wstring(mspf);

			std::wstring windowText = m_MainWndCaption +
				L"    fps: " + fpsStr +
				L"   mspf: " + mspfStr;

			SetWindowText(g_hWnd, windowText.c_str());

			// Reset for next average.
			frameCnt = 0;
			timeElapsed += 1.0f;
		}
	}

	void InitializeApplication(IGameApp& game)
	{
		g_COMInitializer = new COMInitializer();
		InputSystem* inputSystem = new InputSystem();

		Graphics::Initialize();
		GameTimer::Initialize();
		game.Initialize();
		GameTimer::Reset();
	}

	void UpdateApplication(IGameApp& game)
	{
		GameTimer::Tick();

		CalculateFrameStats();

		Display::Update();

		game.Update();
		game.Draw();

		Display::Present();
	}

	void TerminateApplication(IGameApp& game)
	{
		game.Shutdown();
		Graphics::Shutdown();

		GameTimer::Stop();

		delete g_COMInitializer;
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

		g_hWnd = CreateWindow(className, m_MainWndCaption.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
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

	template<typename T>
	inline int GetValue(T value, size_t numMoveBit)
	{
		
		return (int)(short)((uint16_t)((((uint64_t)(value)) >> numMoveBit) & 0xffff));
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
		case WM_KILLFOCUS:
			InputSystem::GetInstance().ClearState();
			break;
		case WM_KEYDOWN:
			InputSystem::GetInstance().OnKeyPressed(static_cast<KeyCode>(wParam));
			break;
		case WM_KEYUP:
			InputSystem::GetInstance().OnKeyReleased(static_cast<KeyCode>(wParam));
			break;
		case WM_MOUSEMOVE:
		{
			int x = GetValue(lParam, 0);
			int y = GetValue(lParam, 16);
			if (x >= 0 && x < (int)Graphics::g_DisplayWidth && y >= 0 && y < (int)Graphics::g_DisplayHeight)
			{
				InputSystem::GetInstance().OnMouseMove(x, y);
				if (!InputSystem::GetInstance().IsInWindow())
				{
					SetCapture(hWnd);
					InputSystem::GetInstance().OnMouseEnter();
				}
			}
			else
			{
				if (wParam & (MK_LBUTTON | MK_RBUTTON))
				{
					//x = std::max(0, x);
					//x = std::min((int)Graphics::g_DisplayWidth - 1, x);
					//y = std::max(0, y);
					//y = std::min((int)Graphics::g_DisplayHeight - 1, y);
					InputSystem::GetInstance().OnMouseMove(x, y);
				}
				else
				{
					ReleaseCapture();
					InputSystem::GetInstance().OnMouseLeave();
					InputSystem::GetInstance().OnLeftReleased(x, y);
					InputSystem::GetInstance().OnRightReleased(x, y);
				}
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			int x = GetValue(lParam, 0);
			int y = GetValue(lParam, 16);
			InputSystem::GetInstance().OnLeftPressed(x, y);
			SetForegroundWindow(hWnd);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			int x = GetValue(lParam, 0);
			int y = GetValue(lParam, 16);
			InputSystem::GetInstance().OnRightPressed(x, y);
			break;
		}
		case WM_LBUTTONUP:
		{
			int x = GetValue(lParam, 0);
			int y = GetValue(lParam, 16);
			InputSystem::GetInstance().OnLeftReleased(x, y);
			break;
		}
		case WM_RBUTTONUP:
		{
			int x = GetValue(lParam, 0);
			int y = GetValue(lParam, 16);
			InputSystem::GetInstance().OnRightReleased(x, y);
			break;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		return 0;
	}
}
