#pragma once

#include "Types.h"
#include "Uncopyable.h"
#include "SourceStream.h"
#include <vector>

namespace mapi {

class MediaSource : Uncopyable
{
public:
	MediaSource();
	~MediaSource();

	MediaSource(MediaSource const&) = delete;
	MediaSource& operator=(MediaSource const&) = delete;

	void LoadFile(const char* u8_url, Error& err) noexcept;
	uint32_t GetStreamCount() noexcept;
	SourceStream* GetStream(uint32_t index) noexcept;

private:
	using SourceStreamArray = std::vector<std::unique_ptr<SourceStream>>;

	std::shared_ptr<AVFormatContext>	m_avfx;
	SourceStreamArray					m_streams;
};

} // End of namespace
