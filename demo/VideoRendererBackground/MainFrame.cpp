#include "framework.h"
#include "MainFrame.h"
#include "AboutDlg.h"
#include <shellapi.h>

#pragma comment(lib, "MediaApi.lib")

MainFrame::MainFrame()
{
}

MainFrame::~MainFrame()
{
}

LRESULT MainFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UpdataLayout();

	m_view.handle = m_hWnd;
	m_graphics = MAPI_Graphics_Create();

	MAPI_Error err{};
	MAPI_Graphics_Initialize(m_graphics, &m_view, &err);

	if (err.code != MAPI_NO_ERROR)
	{
		ShowErrorMessage(L"Initialize video render failed: error = %d", err.code);
	}

	return 0;
}

LRESULT MainFrame::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_graphics != nullptr)
	{
		MAPI_Graphics_Destroy(&m_graphics);
	}

	::PostQuitMessage(0);
	return 0;
}

LRESULT MainFrame::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps;
	HDC	hdc = BeginPaint(&ps);
	Render();
	EndPaint(&ps);

	return 0;
}

LRESULT MainFrame::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Render();
	return 1L;
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

void MainFrame::ShowErrorMessage(const wchar_t* format, ...)
{
	wchar_t buffer[512]{};

	va_list args;
	va_start(args, format);
	int count = vswprintf(buffer, _countof(buffer), format, args);
	va_end(args);

	::MessageBox(m_hWnd, buffer, L"FAILURE", MB_OK);
}

void MainFrame::Render()
{
	if (!m_graphics)
	{
		return;
	}

	MAPI_Error err{};
	MAPI_RenderTarget* target = MAPI_Graphics_BeginRender(m_graphics, &err);

	if (err.code != MAPI_NO_ERROR)
	{
		return;
	}

	MAPI_Color_ARGB color{ 255, 255, 0, 0 };
	MAPI_RenderTarget_FillColor(target, &color, nullptr);

	MAPI_Graphic_EndRender(m_graphics, &target, &err);
}


