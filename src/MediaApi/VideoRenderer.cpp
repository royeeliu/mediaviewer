#include "pch.h"
#include "VideoRenderer.h"
#include "ImageView.h"

namespace mapi {

VideoRenderer::VideoRenderer()
{
}

VideoRenderer::~VideoRenderer()
{
}

void VideoRenderer::Initialize(std::unique_ptr<ImageView>&& view, Error& err)
{
	m_view = std::move(view);
}

};
