#include "pch.h"
#include "MediaSource.h"
#include "Common.h"

namespace mapi {

MediaSource::MediaSource()
{
}

MediaSource::~MediaSource()
{
}

void MediaSource::LoadFile(const char* u8_url, Error& err) noexcept
{
	if (m_avfx != nullptr)
	{
		err = { MAPIERR_INVALID_CALL };
		return;
	}

	AVFormatContext* avfx = nullptr;
	int result = avformat_open_input(&avfx, u8_url, nullptr, nullptr);
	RETURN_VOID_IF(!CHECK_FFMPEG_RESULT("avformat_open_input", result, err));

	auto sp_avfx = std::shared_ptr<AVFormatContext>(avfx, [](AVFormatContext* ptr) 
		{
			avformat_close_input(&ptr);
		});
	avfx = nullptr;

	result = avformat_find_stream_info(sp_avfx.get(), nullptr);
	RETURN_VOID_IF(!CHECK_FFMPEG_RESULT("avformat_find_stream_info", result, err));

	m_avfx = std::move(sp_avfx);

	for (uint32_t i = 0; i < m_avfx->nb_streams; i++)
	{
		m_streams.push_back(std::make_unique<SourceStream>(m_avfx, i));
	}
}

uint32_t MediaSource::GetStreamCount() noexcept
{
	return m_avfx->nb_streams;
}

SourceStream* MediaSource::GetStream(uint32_t index) noexcept
{
	return m_streams[index].get();
}

} // End of namespace
