#pragma once

#include "Types.h"
#include "Uncopyable.h"
#include <evr.h>
#include <d3d9.h>
#include <evr9.h>

namespace mapi {

class Presenter : Uncopyable
{
public:
	Presenter();
	~Presenter();

	void Initialize(std::unique_ptr<TargetView>&& view, Error& err);
	void RepaintVideo();

private:
	std::unique_ptr<TargetView>		m_view;
	uint32_t						m_initializeThreadId = 0;
	bool							m_comInitialized = false;
	bool							m_mfStartuped = false;
	CComPtr<IMFVideoRenderer>		m_evr;
	CComPtr<IMFVideoDisplayControl> m_displayCtrl;
	CComPtr<IMFVideoProcessor>		m_videoProcessor;
};

};
