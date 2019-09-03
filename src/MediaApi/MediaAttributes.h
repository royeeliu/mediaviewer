#pragma once

namespace mapi {

class MediaAttributes
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
	virtual ~MediaAttributes() {};
};

} // End of namespace


