#pragma once

#include "Types.h"
#include "Uncopyable.h"
#include "FFmpeg.h"
#include "MediaDescriptor.h"

namespace mapi {

class FFmpegMediaDescriptor : public MediaDescriptor
{
public:
	FFmpegMediaDescriptor(AVStream* stream);
	~FFmpegMediaDescriptor();

public:
	/* MediaDescriptor methods */
	virtual ClassType GetClassType() const override { return ClassType::FFmpeg; }
	virtual MediaType GetMediaType() const override;

public:
	AVCodecParameters* GetCodecParameters() { return m_codec; }

private:
	AVCodecParameters* m_codec = nullptr;
};

} // End of namespace
#pragma once
