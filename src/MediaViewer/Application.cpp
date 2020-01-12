#include "framework.h"
#include "Application.h"
#include "MainFrame.h"
#include "VideoViewer.h"
#include "Common.h"
#include "Dispatcher.h"

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
	ASSERT(SUCCEEDED(hr));

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

	m_mainThreadId = ::GetCurrentThreadId();

	//RECT frameRect = { 0, 0, 800, 600 };
	HMENU hMenu = ::LoadMenu(hinst, MAKEINTRESOURCE(IDC_MEDIAVIEWER));

	HICON hIcon = ::LoadIcon(hinst, MAKEINTRESOURCE(IDI_MEDIAVIEWER));
	HICON hIconSmall = ::LoadIcon(hinst, MAKEINTRESOURCE(IDI_SMALL));

	m_mainFrame = std::make_unique<MainFrame>();
	HWND hwndMainFrame = m_mainFrame->Create(nullptr, &MainFrame::rcDefault, szTitle, 0, 0, hMenu);
	ASSERT(hwndMainFrame != nullptr);

	m_mainFrame->SetIcon(hIcon);
	m_mainFrame->SetIcon(hIconSmall, FALSE);
	m_mainFrame->CenterWindow();
	m_mainFrame->UpdateWindow();

	m_videoViewer = std::make_unique<VideoViewer>(*(m_mainFrame->GetClientView()));

	m_mainFrame->DestroyedEvent.connect([this] { OnMainFrameDestroyed(); });
	m_mainFrame->LoadVideoEvent.connect([this](const wchar_t* fileName) { OnLoadVideo(fileName); });

	MainThread::Post([this] { ParseCommandLine(); });
}

void Application::Run(int show)
{
	m_mainFrame->ShowWindow(show);

	HACCEL hAccelTable = LoadAccelerators(m_hinstance, MAKEINTRESOURCE(IDC_MEDIAVIEWER));

	MSG msg{};

	// 主消息循环:
	while (msg.message != WM_QUIT)
	{
		DWORD waitResult = ::MsgWaitForMultipleObjectsEx(0,
			nullptr,
			INFINITE,
			QS_ALLEVENTS,
			MWMO_ALERTABLE | MWMO_INPUTAVAILABLE);

		if (waitResult != WAIT_IO_COMPLETION)
		{
			while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					break;
				}

				if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
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
	vswprintf_s(buffer, _countof(buffer), format, args);
	va_end(args);

	WPRINTF(L"\nERROR MESSAGE:\n%s\n", buffer);
}

void Application::ShowErrorMessage(const char* format, ...)
{
	char buffer[512]{};

	va_list args;
	va_start(args, format);
	vsprintf_s(buffer, _countof(buffer), format, args);
	va_end(args);

	PRINTF("\nERROR MESSAGE:\n%s\n", buffer);
}

void Application::OnMainFrameDestroyed()
{
	::PostQuitMessage(0);
}

void Application::OnLoadVideo(const wchar_t* fileName)
{
	if (!m_videoViewer)
	{
		SHOW_ERROR_MESSAGE(L"No video viewer!");
		return;
	}

	m_videoViewer->LoadFile(fileName);
}

void Application::ParseCommandLine()
{
	int argCount = 0;
	wchar_t** argList = ::CommandLineToArgvW(GetCommandLine(), &argCount);

	if (argCount > 1)
	{
		wchar_t const* fileName = argList[1];
		OnLoadVideo(fileName);
	}

	::LocalFree(argList);
}
