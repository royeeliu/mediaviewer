#include "pch.h"
#include "FFmpegVideoDecoder.h"
#include "FFmpegMediaDescriptor.h"
#include "UncompressedVideoDescriptor.h"
#include "MediaPacket.h"
#include "MediaFrame.h"
#include "Diagnisis.h"
#include "Common.h"
#include <thread>

namespace mapi {

FFmpegVideoDecoder::FFmpegVideoDecoder()
{
}

FFmpegVideoDecoder::~FFmpegVideoDecoder()
{
	m_avctx.reset();
}

void FFmpegVideoDecoder::Initialize(MediaDescriptor const& media, Graphics const* graphics, Error& err)
{
	auto mediaDesc = dynamic_cast<FFmpegMediaDescriptor const&>(media);
	AVCodecParameters* codepar = mediaDesc.GetCodecParameters();
	AVCodecID id = codepar->codec_id;
	AVCodec* codec = avcodec_find_decoder(id);

	if (codec == nullptr)
	{
		err.code = MAPIERR_FAIL;
		return;
	}

	auto avctx = std::shared_ptr<AVCodecContext>(avcodec_alloc_context3(nullptr), 
		[](AVCodecContext* ptr) { avcodec_free_context(&ptr); });

	if (!avctx)
	{
		err.code = MAPIERR_OUT_OF_MEMORY;
		return;
	}
	
	int result = avcodec_parameters_to_context(avctx.get(), codepar);
	RETURN_VOID_IF(!CHECK_FFMPEG_RESULT("avcodec_parameters_to_context", result, err));

	avctx->thread_count =  (int)std::thread::hardware_concurrency() + 1;
	avctx->thread_type = FF_THREAD_FRAME;

	result = avcodec_open2(avctx.get(), codec, nullptr);
	RETURN_VOID_IF(!CHECK_FFMPEG_RESULT("avcodec_open2", result, err));

	m_avctx = std::move(avctx);
}

void FFmpegVideoDecoder::SendPacket(MediaPacket const& packet, Error& err)
{
	int result = avcodec_send_packet(m_avctx.get(), packet.GetPacket());
	RETURN_VOID_IF(!CHECK_FFMPEG_RESULT("avcodec_send_packet", result, err));
}

std::shared_ptr<MediaFrame> FFmpegVideoDecoder::ReceiveFrame(Error& err)
{
	auto frame = std::shared_ptr<AVFrame>(av_frame_alloc(), 
		[](AVFrame* ptr) { av_frame_free(&ptr); });
	int result = avcodec_receive_frame(m_avctx.get(), frame.get());

	if (result == AVERROR(EAGAIN))
	{
		return std::shared_ptr<MediaFrame>();
	}

	if (!CHECK_FFMPEG_RESULT("avcodec_send_packet", result, err))
	{
		return std::shared_ptr<MediaFrame>();
	}

	return std::make_shared<MediaFrame>(frame);
}

std::shared_ptr<MediaDescriptor> FFmpegVideoDecoder::GetOutputMediaDescriptor() const
{
	return std::make_shared<UncompressedVideoDescriptor>(m_avctx);
}

} // End of namespace mapi
