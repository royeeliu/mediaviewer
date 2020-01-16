#pragma once

#include "TaskHandle.h"
#include "../SchedulerInterface.h"
#include <memory>

namespace Leo {
namespace Tasks {
namespace Details {

class TaskImplBase : public Referencable
{
public:
	explicit
	TaskImplBase(std::shared_ptr<SchedulerInterface> scheduler) noexcept
		: m_scheduler{ std::move(scheduler) }
	{
	}

	~TaskImplBase()
	{
	}

	void ScheduleTask(TaskHandle* handle)
	{
		m_scheduler->Schedule(&TaskHandle::RunChoreBridge, handle);
	}

private:
	std::shared_ptr<SchedulerInterface> m_scheduler;
};

} // end of namespace Details
} // end of namespace Tasks
} // end of namespace Leo
