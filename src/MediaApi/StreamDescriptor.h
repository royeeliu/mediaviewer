#pragma once

#include "Types.h"
#include "Uncopyable.h"
#include "FFmpeg.h"
#include "MediaDescriptor.h"

namespace mapi {

class StreamDescriptor
{
public:
	StreamDescriptor(std::shared_ptr<AVFormatContext> avfx, uint32_t streamIndex);
	~StreamDescriptor();

	uint32_t GetId() const { return m_stream->id; }
	MediaType GetMediaType() const;
	Rational GetTimebase() const;
	std::unique_ptr<MediaDescriptor> GetMediaAttributes() const;

private:
	std::shared_ptr<AVFormatContext>	m_avfx;
	AVStream* const						m_stream;
};

} // End of namespace
