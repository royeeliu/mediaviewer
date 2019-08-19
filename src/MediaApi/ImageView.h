#pragma once

#include "Types.h"
#include "windows.h"

namespace mapi {

class ImageView
{
public:
	ImageView(View const* view);
	~ImageView();

	HWND GetHwnd();

private:
	View const* m_view;
};

};
