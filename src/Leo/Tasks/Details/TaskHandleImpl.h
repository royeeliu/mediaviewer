#pragma once

#include "TaskHandle.h"

namespace Leo {
namespace Tasks {
namespace Details {

template<typename ReturnType, typename Function>
class TaskHandleImpl : public TaskHandle
{
	using TaskImpl = Details::TaskImpl<ReturnType>;
public:
	TaskHandleImpl(TaskImpl* task, Function func)
		: m_task{ task }
		, m_function{ std::move(func) }
	{
	}

	~TaskHandleImpl()
	{
		SafeRelease(m_task);
	}

private:
	/* TaskHandle method */
	virtual void Invoke() const override
	{
		Function func = std::move(m_function);
		m_task->SetResult(func());
	}

private:
	TaskImpl*	m_task = nullptr;
	Function	m_function;
};

} // end of namespace Details
} // end of namespace Tasks
} // end of namespace Leo
