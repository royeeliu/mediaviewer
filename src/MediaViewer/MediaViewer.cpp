// MediaViewer.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "MainFrame.h"
#include "Global.h"

#pragma comment(lib, "Comctl32.lib")
#define MAX_LOADSTRING 100

CComModule	g_Module;


// 全局变量:
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

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

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MEDIAVIEWER, szWindowClass, MAX_LOADSTRING);
	RECT main_frame_rect = { 0, 0, 800, 600 };
	HMENU hMenu = ::LoadMenu(hInstance, MAKEINTRESOURCE(IDC_MEDIAVIEWER));

	HICON hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MEDIAVIEWER));
	HICON hIconSmall = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

	MainFrame mainFrame;
	HWND hwnd = mainFrame.Create(NULL, &main_frame_rect, szTitle, 0, 0, hMenu);
	ATLASSERT(hwnd != NULL);

	mainFrame.SetIcon(hIcon);
	mainFrame.SetIcon(hIconSmall, FALSE);
	mainFrame.CenterWindow();
	mainFrame.ShowWindow(nCmdShow);


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MEDIAVIEWER));

	MSG msg{};

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	g_Module.Term();
	::CoUninitialize();

    return (int) msg.wParam;
}





