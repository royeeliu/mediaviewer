#pragma once

#include "Scheduler.h"
#include "Details/TaskImpl.h"
#include "Details/TaskHandleImpl.h"

namespace Leo {
namespace Tasks {

template<typename T>
class Task;

template<typename ReturnTypeT>
class Task {
public:
	using ReturnType = ReturnTypeT;
	using ResultType = Result<ReturnType>;
	using TaskImplType = Details::TaskImpl<ReturnType>;

public:
	Task() noexcept
	{
	}

	template<typename ParamType>
	__declspec(noinline)
	explicit
	Task(ParamType param)
	{
		Details::ValidateTaskConstructorArgs<ReturnType, ParamType>(param);
		m_impl = new TaskImplType{ GetDefaultScheduler() };
		m_impl->ScheduleTask(new Details::TaskHandleImpl<ReturnType, ParamType>{ AddReference(m_impl), std::move(param) });
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
		m_impl->Wait();
		return m_impl->GetResult();
	}

private:
	void _Clear() noexcept
	{
		SafeRelease(m_impl);
	}

	void _Move(TaskImplType*& impl) noexcept
	{
		LEO_REQUIRE(m_impl == nullptr);
		m_impl = impl;
		impl = nullptr;
	}

private:
	TaskImplType* m_impl = nullptr;
};

} // end of namespace Tasks
} // end of namespace Leo
