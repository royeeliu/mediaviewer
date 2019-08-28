#pragma once

class ClientView
	: public CWindowImpl<ClientView>
{
public:
	ClientView();
	~ClientView();

	BEGIN_MSG_MAP(MainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_DROPFILES, OnDropFiles)
	END_MSG_MAP()

public:
	typedef boost::signals2::signal<void(void)> PaintSignal;
	typedef boost::signals2::signal<void(void)>	DestroyedSignal;

	PaintSignal		PaintEvent;
	DestroyedSignal	DestroyedEvent;

private:
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDropFiles(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
