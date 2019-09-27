#pragma once

#include "Types.h"
#include "Uncopyable.h"
#include "StreamDescriptor.h"
#include <vector>

namespace mapi {

class MediaSource : Uncopyable
{
public:
	MediaSource();
	~MediaSource();

	void LoadFile(const char* u8_url, Error& err) noexcept;
	uint32_t GetStreamCount() noexcept;
	StreamDescriptor* GetStreamDescriptor(uint32_t index) noexcept;
	int32_t FindBestStream(MediaType type) noexcept;
	std::shared_ptr<MediaPacket> ReadNextPacket(Error& err) noexcept;

private:
	using SourceStreamArray = std::vector<std::unique_ptr<StreamDescriptor>>;

	std::shared_ptr<AVFormatContext>	m_avfx;
	SourceStreamArray					m_streams;
};

} // End of namespace
