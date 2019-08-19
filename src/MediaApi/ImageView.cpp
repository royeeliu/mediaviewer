#include "pch.h"
#include "ImageView.h"

namespace mapi {

ImageView::ImageView(View const* view)
	: m_view(view)
{
}

ImageView::~ImageView()
{
}

HWND ImageView::GetHwnd()
{
	return reinterpret_cast<HWND>(m_view->handle);
}

};
