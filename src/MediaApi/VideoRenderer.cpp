
#include "pch.h"
#include "VideoRenderer.h"
#include "Graphics.h"
#include "Diagnisis.h"
#include "Common.h"

namespace mapi {

VideoRenderer::VideoRenderer()
{
}

VideoRenderer::~VideoRenderer()
{
}

void VideoRenderer::Initialize(MediaDescriptor const& media, Graphics& graphics, Error& err)
{
	m_processor = graphics.CreateProcessor(media, err);
	RETURN_VOID_IF(err.code != MAPI_NO_ERROR);

	m_processor->Initialize(media, err);
	RETURN_VOID_IF(err.code != MAPI_NO_ERROR);
}

void VideoRenderer::Present(std::shared_ptr<MediaFrame> frame)
{
	m_processor->Process(*frame);
}

void VideoRenderer::Repaint()
{
}

void VideoRenderer::SetBackgroundColor(Color::ARGB const& color)
{
}

};
