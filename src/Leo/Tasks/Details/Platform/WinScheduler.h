#pragma once

#include "../../SchedulerInterface.h"

namespace Leo {
namespace Tasks {
namespace Details {
namespace Platform {

class WinScheduler : public SchedulerInterface
{
public:
	virtual void Schedule(TaskProc_t, void*) override;
};

} // end of namespace Platform
} // end of namespace Details
} // end of namespace Tasks
} // end of namespace Leo
