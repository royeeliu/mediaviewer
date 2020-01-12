#pragma once

#include "../../Referencable.h"
#include <mutex>
#include <condition_variable>

namespace Leo { namespace Threading { namespace Channels {

template<class T>
class SyncChannel
{
public:
	class SendingEnd;
	class ReceivingEnd;

public:
	SyncChannel();

	~SyncChannel()
	{
		_Clear();
	}

	SyncChannel(SyncChannel&& other) noexcept
	{
		_Move(other.m_impl);
	}

	SyncChannel& operator=(SyncChannel&& other) noexcept
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

private:
	struct Impl;

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
struct SyncChannel<T>::Impl : Referencable
{
	std::mutex				mutex_;
	std::condition_variable cvSend_;
	std::condition_variable cvReceive_;

	bool					sending_ = false;
	T						obj_{};
};

template<class T>
inline SyncChannel<T>::SyncChannel()
	: m_impl{ new Impl{} }
{
}

template<class T>
inline void SyncChannel<T>::_Clear() noexcept
{
	SafeRelease(m_impl);
}

template<class T>
class SyncChannel<T>::SendingEnd
{
	//using SyncChannel = SyncChannel<T>;
	using unique_lock = std::unique_lock<std::mutex>;

public:
	SendingEnd()
	{
	}

	explicit
	SendingEnd(SyncChannel& channel)
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
		_ASSERTE(!m_impl->sending_);

		m_impl->obj_ = std::move(obj);
		m_impl->sending_ = true;
		m_impl->cvSend_.notify_one();

		while (m_impl->sending_)
		{
			m_impl->cvReceive_.wait(lock);
		}
	}

private:
	void _Clear() noexcept
	{
		SafeRelease(m_impl);
	}

	void _Move(SyncChannel::Impl*& impl) noexcept
	{
		_ASSERTE(m_impl == nullptr);
		m_impl = impl;
		impl = nullptr;
	}

private:
	SyncChannel::Impl* m_impl = nullptr;
};

template<class T>
class SyncChannel<T>::ReceivingEnd
{
	using SyncChannel = SyncChannel<T>;
	using unique_lock = std::unique_lock<std::mutex>;

public:
	ReceivingEnd()
	{
	}

	explicit
	ReceivingEnd(SyncChannel& channel)
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

		while (!m_impl->sending_)
		{
			m_impl->cvSend_.wait(lock);
		}

		T obj{};
		std::swap(obj, m_impl->obj_);
		m_impl->sending_ = false;
		m_impl->cvReceive_.notify_one();

		return obj;
	}

	bool HaveMessage() const
	{
		unique_lock lock(m_impl->mutex_);
		return m_impl->sending_;
	}

private:
	void _Clear() noexcept
	{
		SafeRelease(m_impl);
	}

	void _Move(SyncChannel::Impl*& impl) noexcept
	{
		_ASSERTE(m_impl == nullptr);
		m_impl = impl;
		impl = nullptr;
	}

private:
	SyncChannel::Impl* m_impl = nullptr;
};

} // namespace Channels
} // namespace Threading
} // namespace Leo
