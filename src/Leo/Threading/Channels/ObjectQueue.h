#pragma once

#include "../../Referencable.h"
#include <chrono>

namespace Leo { namespace Threading { namespace Channels {

class ObjectQueue
{
public:
	enum class Status
	{
		Ok = 0,
		Timeout = 1,
		Interrupted = 2,
	};

	class SendingEnd;
	class ReceivingEnd;

public:
	explicit
	ObjectQueue(uint32_t size = 1);

	~ObjectQueue()
	{
		_Clear();
	}

	ObjectQueue(ObjectQueue&& other) noexcept
	{
		_Move(other.m_impl);
	}

	ObjectQueue& operator=(ObjectQueue&& other) noexcept
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

	void Interrupt(int code) noexcept;
	void InterruptSendingEnd(int code) noexcept;
	void InterruptReceivingEnd(int code) noexcept;

	void Reset(int code) noexcept;

	void Resize(uint32_t size) noexcept;

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

class ObjectQueue::SendingEnd
{
public:
	SendingEnd()
	{
	}

	~SendingEnd()
	{
		_Clear();
	}

	explicit
	SendingEnd(ObjectQueue& queue);

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

	bool TrySend(Referencable* obj);
	Status Wait(int& code);
	Status WaitFor(std::chrono::milliseconds timeout, int& code);
	Status Send(Referencable* obj, int& code);
	Status SendFor(Referencable* obj, std::chrono::milliseconds timeout, int& code);

private:
	void _Clear() noexcept;

	void _Move(ObjectQueue::Impl*& impl) noexcept
	{
		_ASSERTE(m_impl == nullptr);
		m_impl = impl;
		impl = nullptr;
	}

private:
	ObjectQueue::Impl* m_impl = nullptr;
};

class ObjectQueue::ReceivingEnd
{
public:
	ReceivingEnd()
	{
	}

	~ReceivingEnd()
	{
		_Clear();
	}

	explicit
	ReceivingEnd(ObjectQueue& queue);

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

	bool TryReceive(Referencable*& obj);
	Status Wait(int& code);
	Status WaitFor(std::chrono::milliseconds timeout, int& code);
	Status Receive(Referencable*& obj, int& code);
	Status ReceiveFor(Referencable*& obj, std::chrono::milliseconds timeout, int& code);

private:
	void _Clear() noexcept;

	void _Move(ObjectQueue::Impl*& impl) noexcept
	{
		_ASSERTE(m_impl == nullptr);
		m_impl = impl;
		impl = nullptr;
	}

private:
	ObjectQueue::Impl* m_impl = nullptr;
};


} // namespace Channels
} // namespace Threading
} // namespace Leo
