#pragma once

#include "Types.h"
#include "Uncopyable.h"

namespace mapi {

class MediaSource : Uncopyable
{
public:
	MediaSource();
	~MediaSource();

	MediaSource(MediaSource const&) = delete;
	MediaSource& operator=(MediaSource const&) = delete;

	void LoadFile(const char* u8_url, Error& err) noexcept;

private:
	std::shared_ptr<AVFormatContext> m_avfx;
};

} // End of namespace
