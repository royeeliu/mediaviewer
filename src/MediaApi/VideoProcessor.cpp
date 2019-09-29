#include "pch.h"
#include "VideoProcessor.h"
#include "MediaFrame.h"
#include "MediaDescriptor.h"
#include "UncompressedVideoDescriptor.h"
#include "Diagnisis.h"
#include "Common.h"

namespace mapi {

VideoProcessor::VideoProcessor(DX11Graphics& graphics)
	: m_graphics(graphics)
{
}

VideoProcessor::~VideoProcessor()
{
}

void VideoProcessor::Initialize(MediaDescriptor const& media, Error& err)
{
	HRESULT hr = m_graphics.GetDevice()->QueryInterface(&m_videoDevice);
	RETURN_VOID_IF(!CHECK_COM_RESULT("ID3D11Device::QueryInterface", hr, err));

	hr = m_graphics.GetDeviceContext()->QueryInterface(&m_videoContext);
	RETURN_VOID_IF(!CHECK_COM_RESULT("ID3D11DeviceContext::QueryInterface", hr, err));

	//m_imageWidthInPixels = surfaceDesc.Width;
	//m_imageHeightInPixels = surfaceDesc.Height;
	auto const& mediaDesc = dynamic_cast<UncompressedVideoDescriptor const&>(media);
	m_imageWidth = mediaDesc.m_avctx->width;
	m_imageHeight = mediaDesc.m_avctx->height;

	D3D11_VIDEO_PROCESSOR_CONTENT_DESC contentDesc{};
	contentDesc.InputFrameFormat = D3D11_VIDEO_FRAME_FORMAT_INTERLACED_TOP_FIELD_FIRST;
	contentDesc.InputWidth = m_imageWidth;
	contentDesc.InputHeight = m_imageHeight;
	contentDesc.OutputWidth = m_imageWidth;
	contentDesc.OutputHeight = m_imageHeight;
	contentDesc.Usage = D3D11_VIDEO_USAGE_PLAYBACK_NORMAL;

	hr = m_videoDevice->CreateVideoProcessorEnumerator(&contentDesc, &m_videoProcessorEnumerator);
	RETURN_VOID_IF(!CHECK_COM_RESULT("ID3D11Device::CreateVideoProcessorEnumerator", hr, err));

	uint32_t flags = 0;
	DXGI_FORMAT vpOutputFormat = DXGI_FORMAT_B8G8R8A8_UNORM;

	hr = m_videoProcessorEnumerator->CheckVideoProcessorFormat(vpOutputFormat, &flags);
	RETURN_VOID_IF(!CHECK_COM_RESULT("ID3D11VideoProcessorEnumerator::CheckVideoProcessorFormat", hr, err));

	if ((flags & D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_OUTPUT) == 0)
	{
		err.code = MAPIERR_SYSTERM;
		return;
	}

	//m_rcSrcApp.left = 0;
	//m_rcSrcApp.top = 0;
	//m_rcSrcApp.right = m_uiRealDisplayWidth;
	//m_rcSrcApp.bottom = m_uiRealDisplayHeight;

	D3D11_VIDEO_PROCESSOR_CAPS caps {};
	D3D11_VIDEO_PROCESSOR_RATE_CONVERSION_CAPS convCaps = {};

	hr = m_videoProcessorEnumerator->GetVideoProcessorCaps(&caps);
	RETURN_VOID_IF(!CHECK_COM_RESULT("ID3D11VideoProcessorEnumerator::GetVideoProcessorCaps", hr, err));

	uint32_t index = 0;
	for (uint32_t i = 0; i < caps.RateConversionCapsCount; i++)
	{
		hr = m_videoProcessorEnumerator->GetVideoProcessorRateConversionCaps(i, &convCaps);
		RETURN_VOID_IF(!CHECK_COM_RESULT("ID3D11VideoProcessorEnumerator::GetVideoProcessorRateConversionCaps", hr, err));

		// Check the caps to see which deinterlacer is supported
		if ((convCaps.ProcessorCaps & D3D11_VIDEO_PROCESSOR_PROCESSOR_CAPS_DEINTERLACE_BOB) != 0)
		{
			index = i;
			break;
		}
	}

	hr = m_videoDevice->CreateVideoProcessor(m_videoProcessorEnumerator, index, &m_videoProcessor);
	RETURN_VOID_IF(!CHECK_COM_RESULT("ID3D11Device::CreateVideoProcessorEnumerator", hr, err));

	D3D11_TEXTURE2D_DESC srcTextureDesc{};
	srcTextureDesc.ArraySize = 1;
	srcTextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	srcTextureDesc.Width = m_imageWidth;
	srcTextureDesc.Height = m_imageHeight;
	srcTextureDesc.Format = DXGI_FORMAT_NV12;
	srcTextureDesc.MipLevels = 1;
	srcTextureDesc.SampleDesc.Count = 1;
	srcTextureDesc.SampleDesc.Quality = 0;
	srcTextureDesc.BindFlags = D3D11_BIND_DECODER;
	srcTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	srcTextureDesc.MiscFlags = 0;
	hr = m_graphics.GetDevice()->CreateTexture2D(&srcTextureDesc, nullptr, &m_srcTexture2D);
	RETURN_VOID_IF(!CHECK_COM_RESULT("ID3D11Device::CreateTexture2D", hr, err));

	m_swsctx = std::shared_ptr<SwsContext>(
		sws_getContext(
			m_imageWidth,
			m_imageHeight,
			mediaDesc.m_avctx->pix_fmt,
			m_imageWidth,
			m_imageHeight,
			AV_PIX_FMT_NV12,
			0,
			nullptr,
			nullptr,
			nullptr),
		[](SwsContext* ptr) { sws_freeContext(ptr); });

	if (!m_swsctx)
	{
		err.code = MAPIERR_FAIL;
		return;
	}
}

void VideoProcessor::Process(MediaFrame const& frame)
{
	Error err{ MAPI_NO_ERROR };
	D3D11_MAPPED_SUBRESOURCE mappedResource{};
	HRESULT hr = m_graphics.GetDeviceContext()->Map(m_srcTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	RETURN_VOID_IF(!CHECK_COM_RESULT("ID3D11Device::CreateTexture2D", hr, err));

	do
	{
		uint32_t const pitch = mappedResource.RowPitch;
		uint32_t const lines = m_imageHeight;
		uint32_t const Y_size = pitch * lines;
		uint32_t const UV_size = pitch * ((lines + 1) / 2);
		uint32_t const dstImageSize = Y_size + UV_size;

		if (m_imageBuffer.size() < dstImageSize)
		{
			m_imageBuffer.resize(dstImageSize);
		}

		uint8_t* dst[] = { m_imageBuffer.data(), m_imageBuffer.data() + Y_size };
		int stride[] = { static_cast<int>(pitch), static_cast<int>(pitch) };

		int fferr = sws_scale(
			m_swsctx.get(),
			frame.m_frame->data,
			frame.m_frame->linesize,
			0,
			m_imageHeight,
			dst,
			stride
			);

		if (fferr <= 0)
		{
			break;
		}

		memcpy_s(mappedResource.pData, dstImageSize, m_imageBuffer.data(), dstImageSize);

	} while (0);

	m_graphics.GetDeviceContext()->Unmap(m_srcTexture2D, 0);

	// Get Backbuffer
	m_graphics.UpdateSwapChain();
	CComPtr<ID3D11Texture2D> swapChainBackBuffer;
	hr = m_graphics.GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)& swapChainBackBuffer);
	RETURN_VOID_IF(!CHECK_COM_RESULT("IDXGISwapChain::GetBuffer", hr, err));

	D3D11_VIDEO_PROCESSOR_OUTPUT_VIEW_DESC outputViewDesc{};
	outputViewDesc.ViewDimension = D3D11_VPOV_DIMENSION_TEXTURE2D;
	outputViewDesc.Texture2D.MipSlice = 0;
	outputViewDesc.Texture2DArray.MipSlice = 0;
	outputViewDesc.Texture2DArray.FirstArraySlice = 0;

	CComPtr<ID3D11VideoProcessorOutputView> outputView;
	hr = m_videoDevice->CreateVideoProcessorOutputView(swapChainBackBuffer, m_videoProcessorEnumerator, &outputViewDesc, &outputView);
	RETURN_VOID_IF(!CHECK_COM_RESULT("ID3D11VideoDevice::CreateVideoProcessorOutputView", hr, err));

	D3D11_VIDEO_PROCESSOR_INPUT_VIEW_DESC inputLeftViewDesc{};
	inputLeftViewDesc.FourCC = 0;
	inputLeftViewDesc.ViewDimension = D3D11_VPIV_DIMENSION_TEXTURE2D;
	inputLeftViewDesc.Texture2D.MipSlice = 0;
	inputLeftViewDesc.Texture2D.ArraySlice = 0;

	CComPtr<ID3D11VideoProcessorInputView> leftInputView;
	hr = m_videoDevice->CreateVideoProcessorInputView(m_srcTexture2D, m_videoProcessorEnumerator, &inputLeftViewDesc, &leftInputView);
	RETURN_VOID_IF(!CHECK_COM_RESULT("ID3D11VideoDevice::CreateVideoProcessorInputView", hr, err));

	D3D11_TEXTURE2D_DESC backBufferDesc{};
	swapChainBackBuffer->GetDesc(&backBufferDesc);

	RECT sourceRect{ 0, 0, static_cast<long>(m_imageWidth), static_cast<long>(m_imageHeight) };
	RECT targetRect = GetTargetRect(m_imageWidth, m_imageHeight, backBufferDesc.Width, backBufferDesc.Height);
	SetVideoContextParameters(sourceRect, targetRect);

	D3D11_VIDEO_PROCESSOR_STREAM streamData{};
	streamData.Enable = TRUE;
	streamData.OutputIndex = 0;
	streamData.InputFrameOrField = 0;
	streamData.PastFrames = 0;
	streamData.FutureFrames = 0;
	streamData.ppPastSurfaces = nullptr;
	streamData.ppFutureSurfaces = nullptr;
	streamData.pInputSurface = leftInputView;
	streamData.ppPastSurfacesRight = nullptr;
	streamData.ppFutureSurfacesRight = nullptr;

	hr = m_videoContext->VideoProcessorBlt(m_videoProcessor, outputView, 0, 1, &streamData);
	RETURN_VOID_IF(!CHECK_COM_RESULT("ID3D11VideoContext::VideoProcessorBlt", hr, err));

	m_graphics.GetSwapChain()->Present(0, 0);
}

void VideoProcessor::SetVideoContextParameters(RECT const& sourceRect, RECT const& targetRect)
{
	D3D11_VIDEO_FRAME_FORMAT frameFormat = D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE;
#if 0
	if (MFVideoInterlace_FieldInterleavedUpperFirst == unInterlaceMode || MFVideoInterlace_FieldSingleUpper == unInterlaceMode || MFVideoInterlace_MixedInterlaceOrProgressive == unInterlaceMode)
	{
		FrameFormat = D3D11_VIDEO_FRAME_FORMAT_INTERLACED_TOP_FIELD_FIRST;
	}
	else if (MFVideoInterlace_FieldInterleavedLowerFirst == unInterlaceMode || MFVideoInterlace_FieldSingleLower == unInterlaceMode)
	{
		FrameFormat = D3D11_VIDEO_FRAME_FORMAT_INTERLACED_BOTTOM_FIELD_FIRST;
	}
#endif

	// input format
	m_videoContext->VideoProcessorSetStreamFrameFormat(m_videoProcessor, 0, frameFormat);

	// Output rate (repeat frames)
	m_videoContext->VideoProcessorSetStreamOutputRate(m_videoProcessor, 0, D3D11_VIDEO_PROCESSOR_OUTPUT_RATE_NORMAL, TRUE, nullptr);

	// Source rect
	m_videoContext->VideoProcessorSetStreamSourceRect(m_videoProcessor, 0, TRUE, &sourceRect);

	// Stream dest rect
	m_videoContext->VideoProcessorSetStreamDestRect(m_videoProcessor, 0, TRUE, &targetRect);

	m_videoContext->VideoProcessorSetOutputTargetRect(m_videoProcessor, TRUE, &targetRect);

	// Stream color space
	D3D11_VIDEO_PROCESSOR_COLOR_SPACE colorSpace = {};
	colorSpace.YCbCr_xvYCC = 1;
	m_videoContext->VideoProcessorSetStreamColorSpace(m_videoProcessor, 0, &colorSpace);

	// Output color space
	m_videoContext->VideoProcessorSetOutputColorSpace(m_videoProcessor, &colorSpace);

	// Output background color (black)
	D3D11_VIDEO_COLOR backgroundColor = {};
	backgroundColor.RGBA.A = 1.0F;
	backgroundColor.RGBA.R = 1.0F * static_cast<float>(GetRValue(0)) / 255.0F;
	backgroundColor.RGBA.G = 1.0F * static_cast<float>(GetGValue(0)) / 255.0F;
	backgroundColor.RGBA.B = 1.0F * static_cast<float>(GetBValue(0)) / 255.0F;

	m_videoContext->VideoProcessorSetOutputBackgroundColor(m_videoProcessor, FALSE, &backgroundColor);
}

RECT VideoProcessor::GetTargetRect(int imageWidth, int imageHeight, int displayWidth, int displayHeight)
{
	if ((int64_t)imageWidth * displayHeight > (int64_t)displayWidth* imageHeight)
	{
		int targetWidth = displayWidth;
		int targetHeight = (int32_t)((int64_t)displayWidth * imageHeight / imageWidth);
		int top = (displayHeight - targetHeight) / 2;
		int bottom = top + targetHeight;
		return RECT{ 0, top, targetWidth, bottom };
	}
	else
	{
		int targetHeight = displayHeight;
		int targetWidth = (int32_t)((int64_t)displayHeight * imageWidth / imageHeight);
		int left = (displayWidth - targetWidth) / 2;
		int right = left + targetWidth;
		return RECT{ left, 0, right, targetHeight };
	}
}


} // End of namespace mapi
