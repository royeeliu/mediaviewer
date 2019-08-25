#include "framework.h"
#include "MainFrame.h"
#include "Application.h"
#include <shellapi.h>

MainFrame::MainFrame()
{
}

MainFrame::~MainFrame()
{
}

LRESULT MainFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ModifyStyleEx(0, WS_EX_ACCEPTFILES);

	//::SetParent(g_hwndConsole, m_hWnd);
	//::SetWindowLong(g_hwndConsole, GWL_STYLE, WS_CHILD);
	//m_hwndClient = g_hwndConsole;

	//DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	//HWND hwndStatusBar = ::CreateWindow(STATUSCLASSNAME, NULL, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, (HMENU)IDW_MAINFRAME_STATUS_BAR, NULL, NULL);

	UpdataLayout();
	::ShowWindow(m_hwndClient, SW_SHOW);

	printf("Hello, World!\n");

	return 0;
}

LRESULT MainFrame::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostQuitMessage(0);
	return 0;
}

LRESULT MainFrame::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT MainFrame::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0L;
}

LRESULT MainFrame::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UpdataLayout();
	return 0;
}

LRESULT MainFrame::OnDropFiles(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDROP hDrop = reinterpret_cast<HDROP>(wParam);

	UINT nNumOfFiles = ::DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);

	if (nNumOfFiles > 0)
	{
		wchar_t szFileName[MAX_PATH] = L"";
		::DragQueryFile(hDrop, 0, szFileName, MAX_PATH);

		if (wcslen(szFileName) > 0)
		{
		}
	}
	::DragFinish(hDrop);

	return 0;
}

LRESULT MainFrame::OnExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DestroyWindow();
	return 0;
}

LRESULT MainFrame::OnAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	::DialogBox(Application::Current.GetInstance(), 
		MAKEINTRESOURCE(IDD_ABOUTBOX), 
		m_hWnd, 
		&MainFrame::About);
	return 0;
}

INT_PTR MainFrame::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void MainFrame::UpdataLayout()
{
	RECT clientRect{};
	GetClientRect(&clientRect);

	if (m_hwndStatusBar != nullptr && ((DWORD)::GetWindowLong(m_hwndStatusBar, GWL_STYLE) & WS_VISIBLE))
	{
		::SendMessage(m_hwndStatusBar, WM_SIZE, 0, 0);
		RECT sbarRect = { 0 };
		::GetWindowRect(m_hwndStatusBar, &sbarRect);
		clientRect.bottom -= sbarRect.bottom - sbarRect.top;
	}

	if (m_hwndClient != nullptr)
	{
		::SetWindowPos(m_hwndClient, 
			nullptr, 
			clientRect.left, 
			clientRect.top, 
			clientRect.right - clientRect.left, 
			clientRect.bottom - clientRect.top, 
			SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

void MainFrame::ShowErrorMessage(const wchar_t* format, ...)
{
	wchar_t buffer[512]{};

	va_list args;
	va_start(args, format);
	int count = vswprintf(buffer, _countof(buffer), format, args);
	va_end(args);

	wprintf(L"\nERROR MESSAGE:\n%s\n", buffer);
}


