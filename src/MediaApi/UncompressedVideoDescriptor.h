#pragma once

#include "Types.h"
#include "Uncopyable.h"
#include "MediaDescriptor.h"
#include "FFmpeg.h"

namespace mapi {

class UncompressedVideoDescriptor
	: Uncopyable
	, public MediaDescriptor
{
public:
	UncompressedVideoDescriptor(std::shared_ptr<AVCodecContext> avctx);
	~UncompressedVideoDescriptor();

public:
	/* MediaDescriptor methods */
	virtual ClassType GetClassType() const override { return MediaDescriptor::ClassType::UncopressedVideo; }
	virtual MediaType GetMediaType() const override { return MediaType::Video; }

public:
	std::shared_ptr<AVCodecContext> m_avctx;
};

} // End of namespace mapi
