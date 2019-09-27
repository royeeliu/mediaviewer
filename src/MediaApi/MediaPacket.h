#pragma once

#include "Types.h"
#include "Uncopyable.h"
#include "FFmpeg.h"

namespace mapi {

class MediaPacket : Uncopyable
{
public:
	MediaPacket(AVPacket*& packet, AVRational const& timebase);
	~MediaPacket();

	int32_t GetStreamIndex() const { return m_packet->stream_index; }
	int64_t GetPts() const { return m_packet->pts; }
	int64_t GetDts() const { return m_packet->dts; }
	Rational const& GetTimebase() const { return m_timebase; }

public:
	AVPacket* GetPacket() const { return m_packet; }
	
private:
	AVPacket* m_packet = nullptr;
	Rational m_timebase{};
};
	
} // End of namespace
