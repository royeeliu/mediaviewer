#pragma once

#include "Types.h"
#include "windows.h"

namespace mapi {

class TargetView
{
public:
	TargetView(MAPI_TargetView const* view);
	~TargetView();

	HWND GetHwnd();

private:
	MAPI_TargetView const* m_view;
};

};
