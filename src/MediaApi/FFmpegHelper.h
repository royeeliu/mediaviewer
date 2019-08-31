#include "Types.h"
#include "FFmpeg.h"

namespace mapi { namespace FFmpegHelper {

inline MediaType ConvertMediaType(AVMediaType type)
{
	switch (type)
	{
	case AVMEDIA_TYPE_VIDEO:
		return MediaType::Video;
	case AVMEDIA_TYPE_AUDIO:
		return MediaType::Audio;
	default:
		return MediaType::Unknown;
	}
}

inline AVMediaType ConvertMediaType(MediaType type)
{
	switch (type)
	{
	case MediaType::Video:
		return AVMEDIA_TYPE_VIDEO;
	case MediaType::Audio:
		return AVMEDIA_TYPE_AUDIO;
	default:
		return AVMEDIA_TYPE_UNKNOWN;
	}
}

inline Rational ConvertRational(AVRational const& value)
{
	return Rational{ value.num, value.den };
}


} } // namespace mapi
