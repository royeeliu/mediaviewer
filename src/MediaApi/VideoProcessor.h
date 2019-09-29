#pragma once

#include "Types.h"
#include "Uncopyable.h"
#include "FFmpeg.h"
#include "DX11Graphics.h"
#include <vector>

namespace mapi {

class VideoProcessor : Uncopyable
{
public:
	VideoProcessor(DX11Graphics& graphics);
	~VideoProcessor();

	void Initialize(MediaDescriptor const& media, Error& err);
	void Process(MediaFrame const& frame);

private:
	void SetVideoContextParameters(RECT const& sourceRect, RECT const& targetRect);
	RECT GetTargetRect(int imageWidth, int imageHeight, int displayWidth, int displayHeight);

private:
	DX11Graphics&							m_graphics;
	CComPtr<ID3D11VideoDevice>				m_videoDevice;
	CComPtr<ID3D11VideoContext>				m_videoContext;
	CComPtr<ID3D11VideoProcessorEnumerator> m_videoProcessorEnumerator;
	CComPtr<ID3D11VideoProcessor>			m_videoProcessor;
	CComPtr<ID3D11Texture2D>				m_srcTexture2D;
	std::shared_ptr<SwsContext>				m_swsctx;

	uint32_t								m_imageWidth = 0;
	uint32_t								m_imageHeight = 0;
	std::vector<uint8_t>					m_imageBuffer;
};

} // End of namespace mapi
