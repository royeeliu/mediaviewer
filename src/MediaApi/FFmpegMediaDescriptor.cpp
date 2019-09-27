#include "pch.h"
#include "StreamDescriptor.h"
#include "FFmpegHelper.h"
#include "Diagnisis.h"
#include "Common.h"
#include "FFmpegMediaDescriptor.h"

namespace mapi {

FFmpegMediaDescriptor::FFmpegMediaDescriptor(AVStream* stream)
{
	m_codec = avcodec_parameters_alloc();
	int result = avcodec_parameters_copy(m_codec, stream->codecpar);
	_ENSURE(result >= 0);
}

FFmpegMediaDescriptor::~FFmpegMediaDescriptor()
{
	avcodec_parameters_free(&m_codec);
}

MediaType FFmpegMediaDescriptor::GetMediaType()  const
{
	return FFmpegHelper::ConvertMediaType(m_codec->codec_type);
}

} // End of namespace
