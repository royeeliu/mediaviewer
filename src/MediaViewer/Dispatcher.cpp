#include "framework.h"
#include "Dispatcher.h"
#include "Application.h"

class ThreadApcScheduler : public concurrency::scheduler_interface
{
public:
	ThreadApcScheduler(uint32_t threadId)
		: m_threadId(threadId)
	{
		m_threadHandle = ::OpenThread(THREAD_ALL_ACCESS, FALSE, m_threadId);
	}

	~ThreadApcScheduler()
	{
		::CloseHandle(m_threadHandle);
		m_threadHandle = nullptr;
	}

	/* scheduler_interface */
	virtual void schedule(concurrency::TaskProc_t proc, void* param) override
	{
		::QueueUserAPC(&ThreadApcScheduler::TaskProc,
			m_threadHandle,
			reinterpret_cast<ULONG_PTR>(new Task{ proc, param }));
	}

private:
	struct Task
	{
		concurrency::TaskProc_t proc;
		void* param;
	};

	static void __stdcall TaskProc(ULONG_PTR Parameter)
	{
		Task* ptr = reinterpret_cast<Task*>(Parameter);
		ptr->proc(ptr->param);
		delete ptr;
	}

private:
	uint32_t				m_threadId = 0;
	HANDLE					m_threadHandle = nullptr;
};

namespace MainThread {

std::shared_ptr<concurrency::scheduler_interface> GetScheduler()
{
	return std::make_shared<ThreadApcScheduler>(Application::Current.GetMainThreadId());
}

}
