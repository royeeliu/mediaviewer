#pragma once

#include "Types.h"
#include "Uncopyable.h"

namespace mapi {

class Graphics : Uncopyable
{
public:
	Graphics();
	~Graphics();

	void Initialize(std::unique_ptr<TargetView>&& target_view, Error& err) noexcept;
	RenderTarget* BeginRendering(Error& err) noexcept;
	void EndRendering(RenderTarget*& target, Error& err) noexcept;

private:
	void Clear();

private:
	uint32_t						m_initializeThreadId = 0;
	std::unique_ptr<TargetView>		m_view;
	std::unique_ptr<DX11Graphics>	m_graphics;
	std::unique_ptr<RenderTarget>	m_renderTarget;
};

};
