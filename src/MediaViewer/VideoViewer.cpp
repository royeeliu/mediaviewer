#include "framework.h"
#include "VideoViewer.h"
#include "ClientView.h"
#include "Common.h"

#pragma comment(lib, "MediaApi.lib")


VideoViewer::VideoViewer(ClientView& displayWindow)
	: m_displayWindow(displayWindow)
{
	m_targetView.handle = (HWND)m_displayWindow;
	m_graphics = MAPI_Graphics_Create();

	m_connections.push_back(m_displayWindow.paintSlignal.connect([this] { Render(); }));

	Initialize();
}

VideoViewer::~VideoViewer()
{
	if (m_graphics != nullptr)
	{
		MAPI_Graphics_Destroy(&m_graphics);
	}

	for (auto& item : m_connections)
	{
		item.disconnect();
	}
}

void VideoViewer::Initialize()
{
	MAPI_Error err{};
	MAPI_Graphics_Initialize(m_graphics, &m_targetView, &err);

	if (err.code != MAPI_NO_ERROR)
	{
		SHOW_ERROR_MESSAGE(L"Initialize video render failed: error = %d", err.code);
	}

	m_initialized = true;
}

void VideoViewer::Render()
{
	if (!m_graphics)
	{
		return;
	}

	MAPI_Error err{};
	MAPI_RenderTarget* target = MAPI_Graphics_BeginRender(m_graphics, &err);

	if (err.code != MAPI_NO_ERROR)
	{
		SHOW_ERROR_MESSAGE("MAPI_Graphics_BeginRender failed: {%d, %d, %s}", err.code, err.detail, err.message);
		return;
	}

	MAPI_RenderTarget_FillColor(target, &m_bkgColor, nullptr);
	MAPI_Graphic_EndRender(m_graphics, &target, &err);
}
