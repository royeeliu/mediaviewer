#include "pch.h"
#include "MediaFrame.h"

namespace mapi {

MediaFrame::MediaFrame(std::shared_ptr<AVFrame> frame)
	: m_frame(std::move(frame))
{
}

MediaFrame::~MediaFrame()
{
}

} // End of namespace mapi
