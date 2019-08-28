#pragma once

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
		_ASSERTE(ref > 1);
		return ref;
	}

	virtual int Release() noexcept final
	{
		int ref = --m_ref;
		_ASSERTE(ref >= 0);

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

	ReferenceGuard(ReferenceGuard const& other)
	{
		_Assign(other.m_ptr);
	}

	ReferenceGuard(ReferenceGuard&& other)
	{
		_Move(other.m_ptr);
	}

	ReferenceGuard& operator=(ReferenceGuard const& other)
	{
		if (&other != this)
		{
			_Clear();
			_Assign(other.m_ptr);
		}

		return *this;
	}

	ReferenceGuard& operator=(ReferenceGuard&& other)
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

	T& operator*() const
	{
		_ASSERTE(m_ptr != nullptr);
		return *m_ptr;
	}

	T* Get() const
	{
		return m_ptr;
	}

	T** GetAdress() 
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
		_ASSERTE(m_ptr == nullptr);
		m_ptr = AddReference(ptr);
	}

	void _Move(T*& ptr)
	{
		_ASSERTE(m_ptr == nullptr);
		m_ptr = ptr;
		ptr = nullptr;
	}

private:
	T* m_ptr = nullptr;
};

} // namespace Leo
