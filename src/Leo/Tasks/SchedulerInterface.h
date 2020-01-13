#pragma once


namespace Leo {
namespace Tasks {

using TaskProc_t = void(__cdecl*)(void*);

struct __declspec(novtable) SchedulerInterface
{
	virtual void Schedule(TaskProc_t proc, void* param) = 0;
};


} // end of namespace Tasks
} // end of namespace Leo
