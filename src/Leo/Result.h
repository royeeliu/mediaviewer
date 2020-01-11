#pragma once

#include "Error.h"
#include "ResultStatus.h"
#include "Details/ResultImpl.h"

namespace Leo {

template<typename T>
class Result;

template<typename T>
class ResultBase
{
public:
	static Result<T> MakeFailureResult(int errorCode);

	/**
	 * @note 形参定义为 Error&& 时，若传入参数为左值，会被隐式转换为 bool 型（Error 重载了 operator bool）,
	 *       然后 bool 被隐式转换为 int 调用到了上面 int 型参数的重载，太坑了！
	 *       所以这里的形参只能定义为 (Error error)
	 */
	static Result<T> MakeFailureResult(Error error);
	static Result<T> MakeCanceledResult();

protected:
	explicit
	ResultBase(Details::ResultImplInterface* impl)
		: m_impl{ impl }
	{
		LEO_REQUIRE(m_impl != nullptr);
	}

	~ResultBase()
	{
		_Clear();
	}

	ResultBase(ResultBase const& other) noexcept
	{
		_Assign(other.m_impl);
	}

	ResultBase& operator=(ResultBase const& other) noexcept
	{
		if (&other != this)
		{
			_Clear();
			_Assign(other.m_impl);
		}
		return *this;
	}

public:
	operator bool() const noexcept
	{
		return Succeeded();
	}

	bool Succeeded() const noexcept
	{
		return Status() == ResultStatus::Succeeded;
	}

	bool Failed() const noexcept
	{
		return Status() == ResultStatus::Failed;
	}

	bool Canceled() const noexcept
	{
		return Status() == ResultStatus::Canceled;
	}

	ResultStatus Status() const noexcept
	{
		return m_impl->Status();
	}

	Leo::Error Error() const noexcept
	{
		return Leo::Error{ Failed() ? AddReference(dynamic_cast<Details::FailureResult*>(m_impl)->Error()) : nullptr };
	}

private:
	void _Clear() noexcept
	{
		SafeRelease(m_impl);
	}

	void _Assign(Details::ResultImplInterface* impl) noexcept
	{
		LEO_REQUIRE(m_impl == nullptr);
		m_impl = AddReference(impl);
	}

protected:
	Details::ResultImplInterface* m_impl = nullptr;
};

template<typename T>
class Result final : public ResultBase<T>
{
	using ValueType = typename T;
	using Base = ResultBase<T>;
	friend class Base;

public:
	static Result MakeSucceededResult(ValueType value)
	{
		return Result{ new Details::SuccessedResult<ValueType>{ std::move(value) } };
	}

private:
	explicit
	Result(Details::ResultImplInterface* impl)
		: Base{ impl }
	{
	}

public:
	~Result()
	{
	}

	Result(Result const& other) noexcept
		: Base{ other }
	{
	}

	Result& operator=(Result const& other) noexcept
	{
		Base::operator=(other);
		return *this;
	}

	ValueType Get() const noexcept
	{
		LEO_REQUIRE(Succeeded());
		return dynamic_cast<Details::SuccessedResult<ValueType>*>(m_impl)->value_;
	}
};

template<>
class Result<void> final : public ResultBase<void>
{
	using Base = ResultBase<void>;
	friend class Base;
public:
	static Result MakeSucceededResult()
	{
		return Result{ new Details::SuccessedResult<void>{} };
	}

private:
	explicit
	Result(Details::ResultImplInterface* impl)
		: Base{ impl }
	{
	}

public:
	~Result()
	{
	}

	Result(Result const& other) noexcept
		: Base{ other }
	{
	}

	Result& operator=(Result const& other) noexcept
	{
		Base::operator=(other);
		return *this;
	}

	void Get() const noexcept
	{
		LEO_REQUIRE(Succeeded());
	}
};

template<typename T>
inline Result<T> ResultBase<T>::MakeFailureResult(int errorCode)
{
	LEO_REQUIRE(errorCode != 0);
	return Result<T>{ new Details::FailureResult{ new Details::ErrorImpl{ errorCode } } };
}

template<typename T>
inline Result<T> ResultBase<T>::MakeFailureResult(Leo::Error error)
{
	LEO_REQUIRE(error.Code() != 0);
	return Result<T>{ new Details::FailureResult{ error._Detach() } };
}

template<typename T>
inline Result<T> ResultBase<T>::MakeCanceledResult()
{
	return Result<T>{ new Details::CanceledResult{} };
}

} // end of namespace Leo
