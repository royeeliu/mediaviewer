#include "pch.h"
#include "Decoder.h"
#include "Common.h"
#include "FFmpegVideoDecoder.h"
#include "MediaDescriptor.h"

namespace mapi {

static std::unique_ptr<DecoderInterface> CreateFFmpegVideoDecoder(MediaDescriptor const& media, Graphics const* graphics, Error& err)
{
	auto decoder = std::make_unique<FFmpegVideoDecoder>();
	decoder->Initialize(media, graphics, err);

	if (err.code != MAPI_NO_ERROR)
	{
		decoder.reset();
	}

	return decoder;
}

Decoder::Decoder()
{
}

Decoder::~Decoder()
{
}

void Decoder::Initialize(MediaDescriptor const& media, Graphics const* graphics, Error& err)
{
	MediaType mediaType = media.GetMediaType();

	switch (mediaType)
	{
	case MediaType::Video:
		m_impl = CreateFFmpegVideoDecoder(media, graphics, err);
		break;
	
	default:
		err.code = MAPIERR_NOT_IMPL;
	}
}

void Decoder::SendPacket(MediaPacket const& packet, Error& err)
{
	m_impl->SendPacket(packet, err);
}

std::shared_ptr<MediaFrame> Decoder::ReceiveFrame(Error& err)
{
	return m_impl->ReceiveFrame(err);
}

std::shared_ptr<MediaDescriptor> Decoder::GetMediaDescriptor() const
{
	return m_impl->GetOutputMediaDescriptor();
}

} // End of namespace mapi

