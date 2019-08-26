#include "framework.h"
#include "VideoViewer.h"
#include "Common.h"

#pragma comment(lib, "MediaApi.lib")

VideoViewer::VideoViewer()
{
}

VideoViewer::~VideoViewer()
{
	if (m_graphics != nullptr)
	{
		MAPI_Graphics_Destroy(&m_graphics);
	}
}

void VideoViewer::Initialize(HWND hwnd)
{
	m_view.handle = hwnd;
	m_graphics = MAPI_Graphics_Create();

	MAPI_Error err{};
	MAPI_Graphics_Initialize(m_graphics, &m_view, &err);

	if (err.code != MAPI_NO_ERROR)
	{
		SHOW_ERROR_MESSAGE(L"Initialize video render failed: error = %d", err.code);
	}
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
