#include "framework.h"
#include "MediaPacket.h"
#include "FFmpegHelper.h"

namespace mapi {

MediaPacket::MediaPacket(AVPacket*& packet, AVRational const& timebase)
	: m_packet(packet)
	, m_timebase(FFmpegHelper::ConvertRational(timebase))
{
	packet = nullptr;
}

MediaPacket::~MediaPacket()
{
	if (m_packet != nullptr)
	{
		av_packet_free(&m_packet);
	}
}

} // End of namespace
