#pragma once

#include "Include/MediaApi.h"

class ClientView;

class VideoViewer
{
public:
	explicit
	VideoViewer(ClientView& displayWindow);
	~VideoViewer();

private:
	void Initialize();
	void Render();

private:
	using SignalConnection = boost::signals2::connection;
	using SignalConnectionArray = std::vector<SignalConnection>;

	ClientView&				m_displayWindow;
	MAPI_TargetView			m_targetView{};
	MAPI_Graphics*			m_graphics{ nullptr };
	MAPI_Color_ARGB			m_bkgColor{ 255, 0, 0, 0 };
	bool					m_initialized = false;
	SignalConnectionArray	m_connections;
};
