#include "MyGameApp.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPWSTR /*lpCmdLine*/, _In_ int nCmdShow)
{
	RunApplication(MyGameApp(), L"MyGameApp", hInstance, nCmdShow);
	return 0;
}