#pragma once

namespace mapi {

class MediaDescriptor
{
public:
	enum class ClassType
	{
		None = 0,
		FFmpeg,
	};

public:
	virtual ClassType GetClassType() = 0;

public:
	virtual ~MediaDescriptor() {};
};

} // End of namespace


