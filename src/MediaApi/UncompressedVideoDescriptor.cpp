#include "pch.h"
#include "UncompressedVideoDescriptor.h"

namespace mapi {

UncompressedVideoDescriptor::UncompressedVideoDescriptor(std::shared_ptr<AVCodecContext> avctx)
	: m_avctx(std::move(avctx))
{
}

UncompressedVideoDescriptor::~UncompressedVideoDescriptor()
{
}

} // End of namespace mapi
