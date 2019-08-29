#pragma once

#include "Types.h"
#include "Uncopyable.h"
#include "FFmpeg.h"

namespace mapi {

class SourceStream
{
public:
	SourceStream(std::shared_ptr<AVFormatContext> avfx, uint32_t streamIndex);
	~SourceStream();

	uint32_t GetId() const { return m_stream->id; }
	MediaType GetMediaType() const;

private:
	std::shared_ptr<AVFormatContext>	m_avfx;
	AVStream* const						m_stream;
};

} // End of namespace
