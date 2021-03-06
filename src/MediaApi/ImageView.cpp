#include "pch.h"
#include "ImageView.h"

namespace mapi {

TargetView::TargetView(View const* view)
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
