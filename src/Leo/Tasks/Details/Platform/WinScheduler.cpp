#include "WinScheduler.h"
#include <windows.h>
#include <system_error>

namespace Leo {
namespace Tasks {
namespace Details {
namespace Platform {

struct SchedulerParam
{
	TaskProc_t m_proc = nullptr;
	void* m_param = nullptr;

	SchedulerParam(TaskProc_t proc, void* param) : m_proc(proc), m_param(param) {}

	static DWORD CALLBACK DefaultWorkCallback(LPVOID lpParameter)
	{
		auto schedulerParam = static_cast<SchedulerParam*>(lpParameter);
		schedulerParam->m_proc(schedulerParam->m_param);
		delete schedulerParam;
		return 1;
	}
};

void WinScheduler::Schedule(TaskProc_t proc, void* param)
{
	auto schedulerParam = new SchedulerParam{ proc, param };
	auto work = QueueUserWorkItem(SchedulerParam::DefaultWorkCallback, schedulerParam, WT_EXECUTELONGFUNCTION);

	if (!work)
	{
		delete schedulerParam;
		throw std::system_error{ static_cast<int>(GetLastError()), std::system_category(), "QueueUserWorkItem failed" };
	}
}

} // end of namespace Platform
} // end of namespace Details
} // end of namespace Tasks
} // end of namespace Leo
