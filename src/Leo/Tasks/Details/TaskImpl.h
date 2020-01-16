#pragma once

#include "TaskImplBase.h"
#include "../../Result.h"
#include <future>

namespace Leo {
namespace Tasks {
namespace Details {

// Anything callable is fine
template<typename ReturnType, typename ParamType>
auto IsValidTaskCtor(ParamType param, int, int, int, int) -> decltype(param(), std::true_type()) {}

template<typename ReturnType, typename ParamType>
void ValidateTaskConstructorArgs(ParamType param)
{
	static_assert(std::is_same<decltype(IsValidTaskCtor<ReturnType>(param, 0, 0, 0, 0)), std::true_type>::value,
		"incorrect argument for task constructor; can be a callable object or a task_completion_event"
		);
}

template<typename ReturnType>
class TaskImpl : public TaskImplBase
{
	using ResultType = Result<ReturnType>;
public:
	explicit
	TaskImpl(std::shared_ptr<SchedulerInterface> scheduler) noexcept
		: TaskImplBase{ scheduler }
	{
		m_future = m_promise.get_future();
	}

	~TaskImpl()
	{
	}

	void SetResult(ReturnType result)
	{
		SetResult(ResultType::MakeSucceededResult(std::move(result)));
	}

	void SetResult(ResultType result)
	{
		m_result = std::make_unique<ResultType>(std::move(result));
		m_promise.set_value();
	}

	void Wait()
	{
		m_future.wait();
	}

	ResultType GetResult()
	{
		return *m_result;
	}

private:
	std::unique_ptr<ResultType>	m_result;
	std::promise<void>			m_promise;
	std::future<void>			m_future;
};

template<>
class TaskImpl<void> : public TaskImplBase
{
};

} // end of namespace Details
} // end of namespace Tasks
} // end of namespace Leo
