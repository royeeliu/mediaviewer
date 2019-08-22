#pragma once

namespace mapi {

class Uncopyable
{
protected:
	Uncopyable() = default;
	~Uncopyable() = default;

	Uncopyable(Uncopyable const&) = delete;
	Uncopyable& operator=(Uncopyable const&) = delete;
};

} // End of namespace
