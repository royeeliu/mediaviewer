#pragma once

#include "Resource.h"
#include "ClientView.h"

class MainFrame
	: public CWindowImpl<MainFrame, CWindow, CFrameWinTraits>
{
public:
	MainFrame();
	~MainFrame();

	BEGIN_MSG_MAP(MainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_DROPFILES, OnDropFiles)
		COMMAND_ID_HANDLER(IDM_EXIT, OnExit)
		COMMAND_ID_HANDLER(IDM_ABOUT, OnAbout)
		COMMAND_ID_HANDLER(ID_LOAD_VIDEO, OnLoadVideo)
	END_MSG_MAP()

public:
	ClientView* GetClientView() const;

public:
public:
	typedef boost::signals2::signal<void(const wchar_t*)>	OpenSaveFileSignal;
	typedef boost::signals2::signal<void(void)>				DestroyedSignal;

	OpenSaveFileSignal	LoadVideoEvent;
	DestroyedSignal		DestroyedEvent;

private:
	virtual void OnFinalMessage(HWND hWnd) override;

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDropFiles(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnLoadVideo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

private:
	static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	void UpdataLayout();

private:
	std::unique_ptr<ClientView>	m_clientView;
	std::unique_ptr<CWindow>	m_statusBar;
};