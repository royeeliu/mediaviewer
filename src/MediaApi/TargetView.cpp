#include "pch.h"
#include "TargetView.h"

namespace mapi {

TargetView::TargetView(MAPI_TargetView const* view)
	: m_view(view)
{
}

TargetView::~TargetView()
{
}

HWND TargetView::GetHwnd()
{
	return reinterpret_cast<HWND>(m_view->handle);
}

};
