#pragma once

#include "Types.h"

namespace mapi {

class MediaDescriptor
{
public:
	enum class ClassType
	{
		None = 0,
		FFmpeg,
		UncopressedVideo,
	};

public:
	virtual ClassType GetClassType() const = 0;
	virtual MediaType GetMediaType() const = 0;

protected:
	virtual ~MediaDescriptor() {};
};

} // End of namespace
