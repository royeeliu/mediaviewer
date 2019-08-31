#include "pch.h"
#include "MediaSource.h"
#include "Common.h"
#include "FFmpegHelper.h"
#include "MediaPacket.h"

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
		m_streams.push_back(std::make_unique<StreamDescriptor>(m_avfx, i));
	}
}

uint32_t MediaSource::GetStreamCount() noexcept
{
	return m_avfx->nb_streams;
}

StreamDescriptor* MediaSource::GetStream(uint32_t index) noexcept
{
	return m_streams[index].get();
}

int32_t MediaSource::FindBestStream(MediaType type) noexcept
{
	int index = av_find_best_stream(m_avfx.get(),
		FFmpegHelper::ConvertMediaType(type),
		-1,
		-1,
		nullptr,
		0);
	return (index >= 0 ? index : -1);
}

std::unique_ptr<MediaPacket> MediaSource::ReadNextPacket(Error& err) noexcept
{
	AVPacket packet{};
	av_init_packet(&packet);

	std::unique_ptr<MediaPacket> mediaPacket{};

	do 
	{
		int result = av_read_frame(m_avfx.get(), &packet);
		BREAK_IF(!CHECK_FFMPEG_RESULT("av_read_frame", result, err));

		AVPacket* refPacket = av_packet_alloc();
		result = av_packet_ref(refPacket, &packet);

		if (!CHECK_FFMPEG_RESULT("av_packet_ref", result, err))
		{
			av_packet_free(&refPacket);
		}

		mediaPacket = std::make_unique<MediaPacket>(
			refPacket, 
			m_avfx->streams[refPacket->stream_index]->time_base);

	} while (0);

	av_packet_unref(&packet);

	return mediaPacket;
}

} // End of namespace
