#pragma once

#include "Types.h"
#include <evr.h>

namespace mapi {

class VideoRenderer
{
public:
	VideoRenderer();
	~VideoRenderer();

	void Initialize(std::unique_ptr<ImageView>&& view, Error& err);

private:
	std::unique_ptr<ImageView>	m_view;
	uint32_t					m_initializeThreadId = 0;
	bool						m_comInitialized = false;
	bool						m_mfStartuped = false;
	CComPtr<IMFVideoRenderer>	m_evr;
};

};
