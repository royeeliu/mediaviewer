#include "framework.h"
#include "MainFrame.h"
#include "AboutDlg.h"
#include <shellapi.h>


MainFrame::MainFrame()
{
}

MainFrame::~MainFrame()
{
}

LRESULT MainFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UpdataLayout();
	return 0;
}

LRESULT MainFrame::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostQuitMessage(0);
	return 0;
}

LRESULT MainFrame::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps;
	HDC	hdc = BeginPaint(&ps);
	EndPaint(&ps);

	return 0;
}

LRESULT MainFrame::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UpdataLayout();
	return 0;
}

LRESULT MainFrame::OnDropFiles(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDROP hDrop = reinterpret_cast<HDROP>(wParam);

	UINT nNumOfFiles = ::DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

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
	AboutDlg dlg;
	dlg.DoModal();
	return 0;
}

void MainFrame::UpdataLayout()
{
}


