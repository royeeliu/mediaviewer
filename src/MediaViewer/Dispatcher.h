#pragma once

#include <ppltasks.h>
#include <functional>

namespace MainThread {

	std::shared_ptr<concurrency::scheduler_interface> GetScheduler();

	template<typename Function>
	inline void Post(Function func)
	{
		concurrency::create_task(func, concurrency::task_options{ GetScheduler() });
	}
}
