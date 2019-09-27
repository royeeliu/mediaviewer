#pragma once

#include "Types.h"
#include "Uncopyable.h"
#include "FFmpeg.h"

namespace mapi {

class MediaFrame : Uncopyable
{
public:
	MediaFrame(std::shared_ptr<AVFrame> frame);
	~MediaFrame();

public:
	std::shared_ptr<AVFrame> m_frame;
};

} // End of namespace mapi
