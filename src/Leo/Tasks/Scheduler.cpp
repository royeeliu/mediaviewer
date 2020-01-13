#include "Scheduler.h"
#include "Details/Platform/WinScheduler.h"
#include <mutex>

namespace Leo {
namespace Tasks {

using DefaultScheduler = Details::Platform::WinScheduler;

static std::shared_ptr<SchedulerInterface>& GetDefaultSchedulerRef()
{
	static std::shared_ptr<SchedulerInterface> s_scheduler;
	return s_scheduler;
}

static std::mutex& GetDefaultSchedulerMutexRef()
{
	static std::mutex s_mutex;
	return s_mutex;
}

std::shared_ptr<SchedulerInterface> GetDefaultScheduler()
{
	auto scheduler = GetDefaultSchedulerRef();
	if (!scheduler)
	{
		std::unique_lock<std::mutex> lock(GetDefaultSchedulerMutexRef());
		auto& schedulerRef = GetDefaultSchedulerRef();
		if (!schedulerRef)
		{
			schedulerRef = std::make_shared<DefaultScheduler>();
		}
		scheduler = schedulerRef;
	}
	return scheduler;
}

void SetDefaultScheduler(std::shared_ptr<SchedulerInterface> scheduler) noexcept
{
	std::unique_lock<std::mutex> lock(GetDefaultSchedulerMutexRef());
	GetDefaultSchedulerRef() = std::move(scheduler);
}

} // end of namespace Tasks
} // end of namespace Leo
