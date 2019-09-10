#pragma once

#include "Types.h"
#include "Uncopyable.h"
#include "FFmpeg.h"
#include "MediaDescriptor.h"

namespace mapi {

class FFmpegMediaAttributes : public MediaDescriptor
{
public:
	FFmpegMediaAttributes(AVStream* stream);
	~FFmpegMediaAttributes();

	MediaType GetMediaType() const;

public:
	virtual ClassType GetClassType() override { return ClassType::FFmpeg; }

private:
	AVCodecParameters* m_codec = nullptr;
};

} // End of namespace
#pragma once