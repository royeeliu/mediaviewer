#pragma once

#include "Diagnisis.h"
#include "Details/ErrorImpl.h"

namespace Leo {

template<typename T>
class ResultBase;

class Error
{
	template<typename T>
	friend class ResultBase;
public:
	Error() noexcept
	{
	}

	~Error()
	{
		_Clear();
	}

	Error(int code)
		: m_impl{ new Details::ErrorImpl{ code } }
	{
	}

	Error(Error&& other) noexcept
	{
		_Move(other.m_impl);
	}

	Error& operator=(Error&& other) noexcept
	{
		if (&other != this)
		{
			_Clear();
			_Move(other.m_impl);
		}
		return *this;
	}

	operator bool() const noexcept
	{
		return m_impl && (m_impl->code_ != 0);
	}

	int Code() const noexcept
	{
		return (m_impl ? m_impl->code_ : 0);
	}

	void Clear() noexcept
	{
		_Clear();
	}

private:
	explicit
	Error(Details::ErrorImpl* impl)
		: m_impl{ impl }
	{
	}

	void _Clear() noexcept
	{
		SafeRelease(m_impl);
	}

	void _Move(Details::ErrorImpl*& impl) noexcept
	{
		LEO_REQUIRE(m_impl == nullptr);
		m_impl = impl;
		impl = nullptr;
	}

	Details::ErrorImpl* _Detach() noexcept
	{
		auto impl = m_impl;
		m_impl = nullptr;
		return impl;
	}

private:
	Details::ErrorImpl* m_impl = nullptr;
};

} // end of namespace Leo
