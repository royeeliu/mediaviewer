#include "pch.h"
#include "StreamDescriptor.h"
#include "FFmpegMediaAttributes.h"
#include "FFmpegHelper.h"
#include "Diagnisis.h"
#include "Common.h"

namespace mapi {

StreamDescriptor::StreamDescriptor(std::shared_ptr<AVFormatContext> avfx, uint32_t streamIndex)
	: m_avfx(avfx)
	, m_stream(avfx->streams[streamIndex])
{
}

StreamDescriptor::~StreamDescriptor()
{
}

MediaType StreamDescriptor::GetMediaType() const
{
	if (m_stream->codecpar != nullptr)
	{
		return FFmpegHelper::ConvertMediaType(m_stream->codecpar->codec_type);
	}
	return MediaType::Unknown;
}

Rational StreamDescriptor::GetTimebase() const
{
	return FFmpegHelper::ConvertRational(m_stream->time_base);
}

std::unique_ptr<MediaAttributes> StreamDescriptor::GetMediaAttributes() const
{
	return std::make_unique<FFmpegMediaAttributes>(m_stream);
}

} // End of namespace
