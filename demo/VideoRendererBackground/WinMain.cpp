// WinMain.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "MainFrame.h"

#pragma comment(lib, "Comctl32.lib")

enum { MAX_LOADSTRING = 100 };

CComModule	g_Module;
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HRESULT hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hr));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	INITCOMMONCONTROLSEX iccx = { sizeof(INITCOMMONCONTROLSEX), ICC_COOL_CLASSES | ICC_BAR_CLASSES };
	BOOL br = ::InitCommonControlsEx(&iccx);
	ATLASSERT(br);

	hr = g_Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hr));

	wchar_t szTitle[MAX_LOADSTRING];
	::LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

	RECT main_frame_rect = { 0, 0, 800, 600 };
	HMENU hMenu = ::LoadMenu(hInstance, MAKEINTRESOURCE(IDC_MAINFRAME));

	HICON hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINFRAME));
	HICON hIconSmall = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

	MainFrame main_frame;
	HWND hwnd_main_frame = main_frame.Create(NULL, &main_frame_rect, szTitle, 0, 0, hMenu);
	ATLASSERT(hwnd_main_frame != NULL);

	main_frame.ModifyStyleEx(0, WS_EX_ACCEPTFILES);
	main_frame.SetIcon(hIcon);
	main_frame.SetIcon(hIconSmall, FALSE);
	main_frame.CenterWindow();
	main_frame.ShowWindow(nCmdShow);

	MSG msg = { 0 };
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAINFRAME));

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	g_Module.Term();
	::CoUninitialize();

	return (int)msg.wParam;
}



