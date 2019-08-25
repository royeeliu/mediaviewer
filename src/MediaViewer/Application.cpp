#include "framework.h"
#include "Application.h"
#include "MainFrame.h"
#include "VideoView.h"
#include "Common.h"

#define MAX_LOADSTRING 100

CComModule	g_comModule;

Application Application::Current;

Application::Application()
{
}

Application::~Application()
{
}

void Application::Initialize(HINSTANCE hinst)
{
	m_hinstance = hinst;

	HRESULT hr = g_comModule.Init(nullptr, hinst);
	ATLASSERT(SUCCEEDED(hr));

	::AllocConsole();
	m_hwndConsole = ::GetConsoleWindow();
	//::ShowWindow(g_hwndConsole, SW_HIDE);

	// 重定向标准输入输出
	FILE* file = nullptr;
	freopen_s(&file, "conin$", "r+t", stdin);
	freopen_s(&file, "conout$", "w+t", stdout);
	freopen_s(&file, "conout$", "w+t", stderr);

	wchar_t szTitle[MAX_LOADSTRING];
	LoadStringW(hinst, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

	//RECT frameRect = { 0, 0, 800, 600 };
	HMENU hMenu = ::LoadMenu(hinst, MAKEINTRESOURCE(IDC_MEDIAVIEWER));

	HICON hIcon = ::LoadIcon(hinst, MAKEINTRESOURCE(IDI_MEDIAVIEWER));
	HICON hIconSmall = ::LoadIcon(hinst, MAKEINTRESOURCE(IDI_SMALL));

	m_mainFrame = std::make_unique<MainFrame>();
	HWND hwnd = m_mainFrame->Create(nullptr, &MainFrame::rcDefault, szTitle, 0, 0, hMenu);
	ATLASSERT(hwnd != nullptr);

	m_mainFrame->SetIcon(hIcon);
	m_mainFrame->SetIcon(hIconSmall, FALSE);
	m_mainFrame->CenterWindow();
}

void Application::Run(int show)
{
	m_mainFrame->UpdateWindow();
	m_mainFrame->ShowWindow(show);

	HACCEL hAccelTable = LoadAccelerators(m_hinstance, MAKEINTRESOURCE(IDC_MEDIAVIEWER));

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

	g_comModule.Term();
	::CoUninitialize();
}

void Application::ShowErrorMessage(const wchar_t* format, ...)
{
	wchar_t buffer[512]{};

	va_list args;
	va_start(args, format);
	int count = vswprintf(buffer, _countof(buffer), format, args);
	va_end(args);

	WPRINTF(L"\nERROR MESSAGE:\n%s\n", buffer);
}
