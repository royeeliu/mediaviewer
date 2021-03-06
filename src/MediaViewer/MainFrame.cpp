﻿#include "framework.h"
#include "MainFrame.h"
#include "Application.h"
#include "Common.h"
#include <shellapi.h>

MainFrame::MainFrame()
{
}

MainFrame::~MainFrame()
{
}

ClientView* MainFrame::GetClientView() const
{
	return m_clientView.get();
}

void MainFrame::OnFinalMessage(HWND hWnd)
{
	DestroyedEvent();
}

LRESULT MainFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ModifyStyleEx(0, WS_EX_ACCEPTFILES);

	//DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	//HWND hwndStatusBar = ::CreateWindow(STATUSCLASSNAME, NULL, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, (HMENU)IDW_MAINFRAME_STATUS_BAR, NULL, NULL);

	m_clientView = std::make_unique<ClientView>();
	HWND hwndView = m_clientView->Create(m_hWnd, &ClientView::rcDefault, L"VideoView");
	ASSERT(hwndView != nullptr);

	UpdataLayout();

	return 0;
}

LRESULT MainFrame::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
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

LRESULT MainFrame::OnLoadVideo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	const wchar_t* filter = L"MP4 File(*.mp4)\0 *.mp4\0All Files(*.*)\0 *.*\0";
	DWORD flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	CFileDialog fileDlg(
		TRUE,
		nullptr,
		nullptr,
		flags,
		filter,
		m_hWnd);

	if (fileDlg.DoModal() == IDOK)
	{
		if (!::PathFileExists(fileDlg.m_szFileName))
		{
			SHOW_ERROR_MESSAGE(L"File not exists: %s", fileDlg.m_szFileName);
		}
		else
		{
			LoadVideoEvent(fileDlg.m_szFileName);
		}
	}

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

	if (m_statusBar != nullptr && m_statusBar->IsWindowVisible())
	{
		m_statusBar->SendMessage(WM_SIZE);
		RECT sbarRect = { 0 };
		m_statusBar->GetWindowRect(&sbarRect);
		clientRect.bottom -= sbarRect.bottom - sbarRect.top;
	}

	if (m_clientView != nullptr)
	{
		m_clientView->SetWindowPos(nullptr, &clientRect, SWP_NOZORDER | SWP_NOACTIVATE);
	}
}
