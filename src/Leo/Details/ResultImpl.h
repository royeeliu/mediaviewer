#pragma once

#include "ErrorImpl.h"
#include "../ResultStatus.h"

namespace Leo {
namespace Details {

class ResultImplInterface : public Referencable
{
public:
	virtual ResultStatus Status() const noexcept = 0;

protected:
	~ResultImplInterface(){}
};

class CanceledResult : public ResultImplInterface
{
public:
	virtual ResultStatus Status() const noexcept override
	{
		return ResultStatus::Canceled;
	}
};

class FailureResult : public ResultImplInterface
{
public:
	explicit
	FailureResult(ErrorImpl* errorImpl)
		: m_error{ errorImpl }
	{
	}

	~FailureResult()
	{
		SafeRelease(m_error);
	}

	virtual ResultStatus Status() const noexcept override
	{
		return ResultStatus::Failed;
	}

	ErrorImpl* Error() const noexcept
	{
		return m_error;
	}

public:
	ErrorImpl* m_error = nullptr;
};

template<class T>
class SuccessedResult : public ResultImplInterface
{
	using ResultType = typename T;
public:
	explicit
	SuccessedResult(ResultType&& value)
		: value_{ std::move(value) }
	{
	}

	~SuccessedResult()
	{
	}

	virtual ResultStatus Status() const noexcept override
	{
		return ResultStatus::Succeeded;
	}

public:
	ResultType value_;
};

template<>
class SuccessedResult<void> : public ResultImplInterface
{
public:
	virtual ResultStatus Status() const noexcept override
	{
		return ResultStatus::Succeeded;
	}
};

} // end of namespace Details
} // end of namespace Leo
