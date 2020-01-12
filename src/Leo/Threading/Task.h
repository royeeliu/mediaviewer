#pragma once

#include "../Result.h"
#include "Details/TaskImpl.h"

namespace Leo {
namespace Threading {

template<typename T>
class Task;

template<typename ReturnTypeT>
class Task {
public:
	using ReturnType = ReturnTypeT;
	using ResultType = Result<ReturnType>;

public:
	Task() noexcept
	{
	}

	~Task()
	{
		_Clear();
	}

	Task(Task&& other) noexcept
	{
		_Move(other.m_impl);
	}

	Task& operator=(Task&& other) noexcept
	{
		if (&other != this)
		{
			_Clear();
			_Move(other.m_impl);
		}
		return *this;
	}

	operator bool() const
	{
		return m_impl != nullptr;
	}

	ResultType Wait()
	{
		return ResultType{};
	}

private:
	using TaskImpl = Details::TaskImpl<ReturnType>;

	void _Clear() noexcept
	{
		SafeRelease(m_impl);
	}

	void _Move(TaskImpl*& impl) noexcept
	{
		LEO_REQUIRE(m_impl == nullptr);
		m_impl = impl;
		impl = nullptr;
	}

private:
	TaskImpl* m_impl = nullptr;
};

} // end of namespace Threading
} // end of namespace Leo
