#pragma once

#include "Include/MediaApi.h"

class VideoViewer
{
public:
	explicit
	VideoViewer();
	~VideoViewer();

	void Initialize(HWND hwnd);
	void Render();

private:
	MAPI_TargetView		m_view{};
	MAPI_Graphics*		m_graphics{ nullptr };
	MAPI_Color_ARGB		m_bkgColor{ 255, 0, 0, 0 };
};
