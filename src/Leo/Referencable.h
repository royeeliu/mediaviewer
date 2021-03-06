﻿#pragma once

#include "Diagnisis.h"
#include <atomic>

namespace Leo {

class Referencable
{
protected:
	Referencable() = default;
	virtual ~Referencable() = default;

	Referencable(Referencable const&) = delete;
	Referencable& operator=(Referencable const&) = delete;

public:
	virtual int Reference() noexcept final
	{
		int ref = ++m_ref;
		LEO_ENSURE(ref > 1);
		return ref;
	}

	virtual int Release() noexcept final
	{
		int ref = --m_ref;
		LEO_ENSURE(ref >= 0);

		if (ref == 0)
		{
			DeleteSelf();
		}

		return ref;
	}

private:
	virtual void DeleteSelf() noexcept
	{
		delete this;
	}

private:
	std::atomic_int m_ref{ 1 };
};

template<class T>
T* AddReference(T* ptr) noexcept
{
	static_assert(std::is_base_of<Referencable, T>::value, "invalid type");

	if (ptr != nullptr)
	{
		ptr->Reference();
	}
	return ptr;
}

template<class T>
void SafeRelease(T*& ptr) noexcept
{
	static_assert(std::is_base_of<Referencable, T>::value, "invalid type");

	if (ptr)
	{
		T* ptr_ = ptr;
		ptr = nullptr;
		ptr_->Release();
	}
}

template<class T>
class ReferenceGuard
{
public:
	static ReferenceGuard Attach(T* ptr)
	{
		return ReferenceGuard(ptr);
	}

	ReferenceGuard()
	{
	}

	~ReferenceGuard()
	{
		SafeRelease(m_ptr);
	}

	ReferenceGuard(ReferenceGuard const& other) noexcept
	{
		_Assign(other.m_ptr);
	}

	ReferenceGuard(ReferenceGuard&& other) noexcept
	{
		_Move(other.m_ptr);
	}

	ReferenceGuard& operator=(ReferenceGuard const& other) noexcept
	{
		if (&other != this)
		{
			_Clear();
			_Assign(other.m_ptr);
		}

		return *this;
	}

	ReferenceGuard& operator=(ReferenceGuard&& other) noexcept
	{
		if (&other != this)
		{
			_Clear();
			_Move(other.m_ptr);
		}

		return *this;
	}

	operator bool() const
	{
		return m_ptr != nullptr;
	}

	bool operator==(T const* ptr) const
	{
		return m_ptr == ptr;
	}

	bool operator!=(T const* ptr) const
	{
		return m_ptr != ptr;
	}

	T& operator*() const
	{
		_ASSERTE(m_ptr != nullptr);
		return *m_ptr;
	}

	T* operator->() const
	{
		_ASSERTE(m_ptr != nullptr);
		return m_ptr;
	}

	T* Get() const
	{
		return m_ptr;
	}

	T** GetAddress()
	{
		_ASSERTE(m_ptr == nullptr);
		return &m_ptr;
	}

	void Reset()
	{
		_Clear();
	}

	T* Detach()
	{
		T* ptr = m_ptr;
		m_ptr = nullptr;
		return ptr;
	}

private:
	explicit
	ReferenceGuard(T* ptr)
		: m_ptr(ptr)
	{
	}

	void _Clear()
	{
		SafeRelease(m_ptr);
	}

	void _Assign(T* ptr)
	{
		LEO_REQUIRE(m_ptr == nullptr);
		m_ptr = AddReference(ptr);
	}

	void _Move(T*& ptr)
	{
		LEO_REQUIRE(m_ptr == nullptr);
		m_ptr = ptr;
		ptr = nullptr;
	}

private:
	T* m_ptr = nullptr;
};

template<class T>
inline ReferenceGuard<T> MakeReferenceGuard(T*&& ptr)
{
	T* tmpPtr = ptr;
	ptr = nullptr;
	return ReferenceGuard<T>::Attach(tmpPtr);
}

} // namespace Leo
