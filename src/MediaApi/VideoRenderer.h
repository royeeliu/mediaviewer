#pragma once

#include "Types.h"
#include "Uncopyable.h"
#include "FFmpeg.h"
#include "VideoProcessor.h"

namespace mapi {

class VideoRenderer : Uncopyable
{
public:
	VideoRenderer();
	~VideoRenderer();

	void Initialize(MediaDescriptor const& media, Graphics& graphics, Error& err);
	void Present(std::shared_ptr<MediaFrame> frame);
	void Repaint();
	void SetBackgroundColor(Color::ARGB const& color);

private:
	std::unique_ptr<VideoProcessor>		m_processor;
};

} // End of namespace
