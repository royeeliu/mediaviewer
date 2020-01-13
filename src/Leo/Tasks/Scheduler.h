#pragma once

#include "SchedulerInterface.h"
#include <memory>

namespace Leo {
namespace Tasks {

std::shared_ptr<SchedulerInterface> GetDefaultScheduler();
void SetDefaultScheduler(std::shared_ptr<SchedulerInterface> scheduler) noexcept;

} // end of namespace Tasks
} // end of namespace Leo
