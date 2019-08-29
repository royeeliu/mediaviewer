#include "pch.h"
#include "SourceStream.h"
#include "FFmpegHelper.h"

namespace mapi {

SourceStream::SourceStream(std::shared_ptr<AVFormatContext> avfx, uint32_t streamIndex)
	: m_avfx(avfx)
	, m_stream(avfx->streams[streamIndex])
{
}

SourceStream::~SourceStream()
{
}

MediaType SourceStream::GetMediaType() const
{
	if (m_stream->codecpar != nullptr)
	{
		return FFmpegHelper::ConvertMediaType(m_stream->codecpar->codec_type);
	}
	return MediaType::Unknown;
}

} // End of namespace
