#pragma once

#include "TaskImplBase.h"
#include "../Predefine.h"
#include "../../Result.h"

#include <future>

namespace Leo {
namespace Tasks {
namespace Details {

// stuff ported from Dev11 CRT
// NOTE: this doesn't actually match std::declval. it behaves differently for void!
// so don't blindly change it to std::declval.
template<class T>
T&& Declval();

struct BadContinuationParamType
{
};

template<typename Type>
Task<Type> ToTask(Type t);

template<typename Function>
Task<void> ToVoidTask(Function f);

template<typename Function, typename Type>
auto ReturnTypeHelper(Type t, Function func, int, int) -> decltype(func(ToTask(t)));
template<typename Function, typename Type>
auto ReturnTypeHelper(Type t, Function func, int, ...) -> decltype(func(t));
template<typename Function, typename Type>
auto ReturnTypeHelper(Type t, Function func, ...) -> BadContinuationParamType;

template<typename Function, typename Type>
auto IsTaskHelper(Type t, Function func, int, int) -> decltype(func(ToTask(t)), std::true_type());
template<typename Function>

auto VoidReturnTypeHelper(Function func, int, int) -> decltype(func(ToVoidTask(func)));
template<typename Function>
auto VoidReturnTypeHelper(Function func, int, ...) -> decltype(func());

template<typename Function>
auto VoidIsTaskHelper(Function func, int, int) -> decltype(func(ToVoidTask(func)), std::true_type());
template<typename Function>
std::false_type VoidIsTaskHelper(Function func, int, ...);

template<typename Function, typename ExpectedParameterType>
struct FunctionTypeTraits
{
	using FunctionReturnType = decltype(ReturnTypeHelper(Declval<ExpectedParameterType>(), Declval<Function>(), 0, 0)) ;
    static_assert(!std::is_same<FunctionReturnType, BadContinuationParamType>::value,
                  "incorrect parameter type for the callable object in 'then'; consider _ExpectedParameterType or "
                  "task<_ExpectedParameterType> (see below)");
	using TakesTask =  decltype(IsTaskHelper(Declval<ExpectedParameterType>(), Declval<Function>(), 0, 0));
};

template<typename Function>
struct FunctionTypeTraits<Function, void>
{
	using FunctionReturnType = decltype(VoidReturnTypeHelper(Declval<Function>(), 0, 0));
    using TakesTask  = decltype(VoidIsTaskHelper(Declval<Function>(), 0, 0)) ;
};

template<typename T>
struct UnwrapTaskType
{
	using Type = T;
};

template<typename T>
struct UnwrapTaskType<Task<T>>
{
	using Type = T;
};

template<typename Type>
struct TaskTypeTraits
{
	using TaskReturnType = typename UnwrapTaskType<Type>::Type;
};

template<>
struct TaskTypeTraits<void>
{
	using TaskReturnType = void;
};

template<typename Function, typename ReturnType>
struct ContinuationTypeTraits
{
	using TaskType = Task<
		typename TaskTypeTraits<typename FunctionTypeTraits<Function, ReturnType>::FunctionReturnType>::TaskReturnType
	>;
};

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
