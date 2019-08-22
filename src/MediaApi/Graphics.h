#pragma once

#include "Types.h"
#include "Uncopyable.h"

namespace mapi {

class Graphics : Uncopyable
{
public:
	Graphics() = default;
	~Graphics();

	void Initialize(std::unique_ptr<TargetView>&& target_view, Error& err) noexcept;
	std::unique_ptr<RenderTarget> BeginRendering(Error& err) noexcept;
	void EndRendering(std::unique_ptr<RenderTarget>&& target) noexcept;

private:
	std::unique_ptr<TargetView>		m_view;
	uint32_t						m_initializeThreadId = 0;
};

};
