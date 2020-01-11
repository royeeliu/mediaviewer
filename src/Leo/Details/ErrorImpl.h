#pragma once

#include "../Referencable.h"

namespace Leo {
namespace Details {

class ErrorImpl : public Referencable
{
public:
	explicit
	ErrorImpl(int code) noexcept
		: code_ { code }
	{
	}

	~ErrorImpl()
	{
	}

public:
	int const code_;
};

} // end of namespace Details
} // end of namespace Leo
