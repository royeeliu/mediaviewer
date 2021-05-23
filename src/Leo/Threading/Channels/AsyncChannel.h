#pragma once

#include "../../Referencable.h"
#include <mutex>
#include <condition_variable>
#include <list>

namespace Leo { namespace Threading { namespace Channels {

//////////////////////////////////////////////////////////////////////////
// class AsyncChannel
//////////////////////////////////////////////////////////////////////////
template<class T>
class AsyncChannel
{
public:
	class SendingEnd;
	class ReceivingEnd;

public:
	AsyncChannel(uint32_t size = 1);

	~AsyncChannel()
	{
		_Clear();
	}

	AsyncChannel(AsyncChannel&& other) noexcept
	{
		_Move(other.m_impl);
	}

	AsyncChannel& operator=(AsyncChannel&& other) noexcept
	{
		if (&other != this)
		{
			_Clear();
			_Move(other.m_impl);
		}

		return *this;
	}

	operator bool() const
	{
		return m_impl != nullptr;
	}

	void Resize(uint32_t size);
	void Reset();

private:
	struct Impl;
	using unique_lock = std::unique_lock<std::mutex>;

	void _Clear() noexcept;

	void _Move(Impl*& impl) noexcept
	{
		_ASSERTE(m_impl == nullptr);
		m_impl = impl;
		impl = nullptr;
	}

private:
	Impl* m_impl = nullptr;
};

template<class T>
struct AsyncChannel<T>::Impl : Referencable
{
	std::mutex				mutex_;
	std::condition_variable cv_;
	uint32_t				maxSize_ = 1;
	std::list<T>			objects_;

public:
	void SetSize(uint32_t size)
	{
		_ASSERTE(size > 0);
		maxSize_ = size;
	}
};

template<class T>
inline AsyncChannel<T>::AsyncChannel(uint32_t size /*= 1*/)
	: m_impl{new Impl{}}
{
	m_impl->SetSize(size > 0 ? size : 1);
}

template<class T>
inline void AsyncChannel<T>::Resize(uint32_t size)
{
	unique_lock lock(m_impl->mutex_);
	m_impl->Resize(size > 0 ? size : 1);
}

template<class T>
inline void AsyncChannel<T>::Reset()
{
	unique_lock lock(m_impl->mutex_);
	m_impl->objects_.clear();
}

template<class T>
inline void AsyncChannel<T>::_Clear() noexcept
{
	SafeRelease(m_impl);
}

//////////////////////////////////////////////////////////////////////////
// class AsyncCahnnel::SendingEnd
//////////////////////////////////////////////////////////////////////////
template<class T>
class AsyncChannel<T>::SendingEnd
{
	using unique_lock = std::unique_lock<std::mutex>;

public:
	SendingEnd()
	{
	}

	explicit
	SendingEnd(AsyncChannel& channel)
		: m_impl(AddReference(channel.m_impl))
	{
	}

	~SendingEnd()
	{
		_Clear();
	}

	SendingEnd(SendingEnd&& other) noexcept
	{
		_Move(other.m_impl);
	}

	SendingEnd& operator=(SendingEnd&& other) noexcept
	{
		if (&other != this)
		{
			_Clear();
			_Move(other.m_impl);
		}

		return *this;
	}

	operator bool() const
	{
		return m_impl != nullptr;
	}

	void SendAsync(T obj)
	{
		unique_lock lock(m_impl->mutex_);

		while (m_impl->objects_.size() >= m_impl->maxSize_)
		{
			m_impl->cv_.wait(lock);
		}

		m_impl->objects_.push_back(obj);
		m_impl->cv_.notify_one();
	}

private:
	void _Clear() noexcept
	{
		SafeRelease(m_impl);
	}

	void _Move(AsyncChannel::Impl*& impl) noexcept
	{
		_ASSERTE(m_impl == nullptr);
		m_impl = impl;
		impl = nullptr;
	}

private:
	AsyncChannel::Impl* m_impl = nullptr;
};

template<class T>
class AsyncChannel<T>::ReceivingEnd
{
	using unique_lock = std::unique_lock<std::mutex>;

public:
	ReceivingEnd()
	{
	}

	explicit
	ReceivingEnd(AsyncChannel& channel)
		: m_impl(AddReference(channel.m_impl))
	{
	}

	~ReceivingEnd()
	{
		_Clear();
	}

	ReceivingEnd(ReceivingEnd&& other) noexcept
	{
		_Move(other.m_impl);
	}

	ReceivingEnd& operator=(ReceivingEnd&& other) noexcept
	{
		if (&other != this)
		{
			_Clear();
			_Move(other.m_impl);
		}

		return *this;
	}

	operator bool() const
	{
		return m_impl != nullptr;
	}

	T Receive()
	{
		unique_lock lock(m_impl->mutex_);

		while (m_impl->objects_.empty())
		{
			m_impl->cv_.wait(lock);
		}

		T obj{};
		std::swap(obj, m_impl->objects_.front());
		m_impl->objects_.pop_front();
		m_impl->cv_.notify_one();

		return obj;
	}

	bool HaveMessage() const
	{
		unique_lock lock(m_impl->mutex_);
		return !m_impl->objects_.empty();
	}

private:
	void _Clear() noexcept
	{
		SafeRelease(m_impl);
	}

	void _Move(AsyncChannel::Impl*& impl) noexcept
	{
		_ASSERTE(m_impl == nullptr);
		m_impl = impl;
		impl = nullptr;
	}

private:
	AsyncChannel::Impl* m_impl = nullptr;
};

} // namespace Channels
} // namespace Threading
} // namespace Leo
