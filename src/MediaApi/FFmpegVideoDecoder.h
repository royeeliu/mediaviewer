#pragma once

#include "Types.h"
#include "Uncopyable.h"
#include "DecoderInterface.h"
#include "FFmpeg.h"

namespace mapi {

class FFmpegVideoDecoder 
	: Uncopyable
	, public DecoderInterface
{
public:
	FFmpegVideoDecoder();
	~FFmpegVideoDecoder();

	/* DecoderInterface methods */
	virtual void Initialize(MediaDescriptor const& media, Graphics const* graphics, Error& err) override;
	virtual void SendPacket(MediaPacket const& packet, Error& err) override;
	virtual std::shared_ptr<MediaFrame> ReceiveFrame(Error& err) override;
	virtual std::shared_ptr<MediaDescriptor> GetOutputMediaDescriptor() const override;

private:
	std::shared_ptr<AVCodecContext> m_avctx;
};

} // End of namespace mapi

