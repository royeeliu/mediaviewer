// MediaViewer.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Application.h"

#pragma comment(lib, "Comctl32.lib")


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	HRESULT hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hr));

	INITCOMMONCONTROLSEX iccx = { sizeof(INITCOMMONCONTROLSEX), ICC_COOL_CLASSES | ICC_BAR_CLASSES };
	BOOL br = ::InitCommonControlsEx(&iccx);
	ATLASSERT(br);

	Application::Current.Initialize(hInstance);
	Application::Current.Run(nCmdShow);

	return 0;
}





