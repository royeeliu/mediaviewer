#pragma once

#include "Predefine.h"
#include "Scheduler.h"
#include "Details/TaskImpl.h"
#include "Details/InitialTaskHandle.h"

namespace Leo {
namespace Tasks {

template<typename ReturnType>
class Task
{
	using ResultType = Result<ReturnType>;
	using TaskImplType = Details::TaskImpl<ReturnType>;

public:
	Task() noexcept
	{
	}

	template<typename Function>
	__declspec(noinline)
	explicit
	Task(Function func)
	{
		Details::ValidateTaskConstructorArgs<ReturnType, Function>(func);
		m_impl = new TaskImplType{ GetDefaultScheduler() };
		m_impl->ScheduleTask(new Details::InitialTaskHandle<ReturnType, Function>{ AddReference(m_impl), std::move(func) });
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

	template<typename Function>
	__declspec(noinline)
	auto Then(Function&& func) const -> typename Details::ContinuationTypeTraits<Function, ReturnType>::TaskType
	{
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
