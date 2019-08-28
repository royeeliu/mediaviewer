#pragma once

#include "Referencable.h"
#include <chrono>

namespace Leo { namespace Channels {

enum class WaitResult
{
	Ok = 0,
	Timeout = 1,
	Interrupted = 2,
};

class ObjectQueue
{
public:
	class InputEnd;
	class OutputEnd;

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

class ObjectQueue::InputEnd
{
public:
	InputEnd(ObjectQueue& queue);

	~InputEnd()
	{
		_Clear();
	}

	InputEnd(InputEnd&& other) noexcept
	{
		_Move(other.m_impl);
	}

	InputEnd& operator=(InputEnd&& other) noexcept
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

	bool Push(Referencable* obj);
	WaitResult Wait(int& code);
	WaitResult WaitFor(std::chrono::milliseconds timeout, int& code);
	WaitResult WaitPush(Referencable* obj, int& code);
	WaitResult WaitPushFor(Referencable* obj, std::chrono::milliseconds timeout, int& code);

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

class ObjectQueue::OutputEnd
{
public:
	OutputEnd(ObjectQueue& queue);

	~OutputEnd()
	{
		_Clear();
	}

	OutputEnd(OutputEnd&& other) noexcept
	{
		_Move(other.m_impl);
	}

	OutputEnd& operator=(OutputEnd&& other) noexcept
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

	bool Pop(Referencable*& obj);
	WaitResult Wait(int& code);
	WaitResult WaitFor(std::chrono::milliseconds timeout, int& code);
	WaitResult WaitPop(Referencable*& obj, int& code);
	WaitResult WaitPopFor(Referencable*& obj, std::chrono::milliseconds timeout, int& code);

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
} // namespace Leo


