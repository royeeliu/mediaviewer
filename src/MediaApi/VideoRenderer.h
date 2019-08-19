#pragma once

#include "Types.h"

namespace mapi {

class VideoRenderer
{
public:
	VideoRenderer();
	~VideoRenderer();

	void Initialize(std::unique_ptr<ImageView>&& view, Error& err);

private:
	std::unique_ptr<ImageView>	m_view;
};

};
