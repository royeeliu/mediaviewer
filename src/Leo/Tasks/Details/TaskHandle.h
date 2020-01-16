#pragma once

#include "../../Referencable.h"

namespace Leo {
namespace Tasks {
namespace Details {

class TaskHandle : public Referencable
{
public:
	virtual void Invoke() const = 0;

	void operator()() const
	{
		this->Invoke();
	}

	static void __cdecl RunChoreBridge(void* param)
	{
		auto handle = static_cast<TaskHandle*>(param);
		handle->Invoke();
		Leo::SafeRelease(handle);
	}
};

} // end of namespace Details
} // end of namespace Tasks
} // end of namespace Leo
