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

} } // namespace mapi
