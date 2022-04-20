#include "PCH.h"
#include "MyGameApp.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPWSTR /*lpCmdLine*/, _In_ int nCmdShow)
{


	MyGameApp app;
	try
	{
		RunApplication(app, L"MyGameApp", hInstance, nCmdShow);
	}
	catch (Utility::DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
	return 0;
}