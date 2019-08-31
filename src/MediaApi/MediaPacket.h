#pragma once

#include "Types.h"
#include "Uncopyable.h"

namespace mapi {

class MediaPacket
{
public:
	MediaPacket(AVPacket*& packet, AVRational const& timebase);
	~MediaPacket();

	int32_t GetStreamIndex() const { return m_packet->stream_index; }
	int64_t GetPts() const { return m_packet->pts; }
	int64_t GetDts() const { return m_packet->dts; }
	Rational const& GetTimebase() const { return m_timebase; }
	
private:
	AVPacket* m_packet = nullptr;
	Rational m_timebase{};
};
	
} // End of namespace
