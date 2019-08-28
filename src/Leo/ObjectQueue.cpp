#include "Channels.h"
#include <list>
#include <mutex>
#include <condition_variable>

using namespace std::chrono;

namespace Leo { namespace Channels {

using unique_lock = std::unique_lock<std::mutex>;

struct ObjectQueue::Impl : Referencable
{
	uint32_t								maxSize_ = 1;
	std::list<ReferenceGuard<Referencable>> objects_;
	std::list<int>							inputInterrupts_;
	std::list<int>							outputInterrupts_;
	std::mutex								mutex_;
	std::condition_variable					cv_;

public:
	void Resize(int size)
	{
		_ASSERTE(size > 0);
		maxSize_ = size;
	}

	void Interrupt(int code)
	{
		inputInterrupts_.push_back(code);
		outputInterrupts_.push_back(code);
		cv_.notify_one();
	}

	bool Push(Referencable* obj)
	{
		if (objects_.size() >= maxSize_ || !inputInterrupts_.empty())
		{
			return false;
		}

		objects_.push_back(ReferenceGuard<Referencable>::Attach(AddReference(obj)));
		cv_.notify_one();
		return true;
	}

	bool Pop(Referencable*& obj)
	{
		if (objects_.empty() || !outputInterrupts_.empty())
		{
			return false;
		}

		obj = objects_.front().Detach();
		objects_.pop_front();
		cv_.notify_one();
		return true;
	}

	void ClearObjects()
	{
		objects_.clear();
	}

	WaitResult WaitPush(unique_lock& lock, int& code)
	{
		while (true)
		{
			if (!inputInterrupts_.empty())
			{
				code = inputInterrupts_.front();
				inputInterrupts_.pop_front();
				return WaitResult::Interrupted;
			}

			if (objects_.size() < maxSize_)
			{
				return WaitResult::Ok;
			}

			cv_.wait(lock);
		}
	}

	WaitResult WaitPushFor(unique_lock& lock, milliseconds timeout, int& code)
	{
		auto startTime = high_resolution_clock::now();

		while (true)
		{
			if (!inputInterrupts_.empty())
			{
				code = inputInterrupts_.front();
				inputInterrupts_.pop_front();
				return WaitResult::Interrupted;
			}

			if (objects_.size() < maxSize_)
			{
				return WaitResult::Ok;
			}

			auto elapsed = duration_cast<milliseconds>(high_resolution_clock::now() - startTime);

			if (timeout <= elapsed)
			{
				return WaitResult::Timeout;
			}

			std::cv_status status = cv_.wait_for(lock, timeout - elapsed);

			if (status == std::cv_status::timeout)
			{
				return WaitResult::Timeout;
			}
		}
	}

	WaitResult WaitPop(unique_lock& lock, int& code)
	{
		while (true)
		{
			if (!outputInterrupts_.empty())
			{
				code = outputInterrupts_.front();
				outputInterrupts_.pop_front();
				return WaitResult::Interrupted;
			}

			if (!objects_.empty())
			{
				return WaitResult::Ok;
			}

			cv_.wait(lock);
		}
	}

	WaitResult WaitPopFor(unique_lock& lock, milliseconds timeout, int& code)
	{
		auto startTime = high_resolution_clock::now();

		while (true)
		{
			if (!outputInterrupts_.empty())
			{
				code = outputInterrupts_.front();
				outputInterrupts_.pop_front();
				return WaitResult::Interrupted;
			}

			if (!objects_.empty())
			{
				return WaitResult::Ok;
			}

			auto elapsed = duration_cast<milliseconds>(high_resolution_clock::now() - startTime);

			if (timeout <= elapsed)
			{
				return WaitResult::Timeout;
			}

			std::cv_status status = cv_.wait_for(lock, timeout - elapsed);

			if (status == std::cv_status::timeout)
			{
				return WaitResult::Timeout;
			}
		}
	}

};


// class ObjectQueue

ObjectQueue::ObjectQueue(uint32_t size/* = 1*/)
	: m_impl(new Impl{})
{
	m_impl->Resize((size > 0) ? size : 1);
}

void ObjectQueue::Interrupt(int code) noexcept
{
	unique_lock lock(m_impl->mutex_);
	m_impl->Interrupt(code);
}

void ObjectQueue::Reset(int code) noexcept
{
	unique_lock lock(m_impl->mutex_);
	m_impl->ClearObjects();
	m_impl->Interrupt(code);
}

void ObjectQueue::Resize(uint32_t size) noexcept
{
	unique_lock lock(m_impl->mutex_);
	m_impl->Resize((size > 0) ? size : 1);
}

void ObjectQueue::_Clear() noexcept
{
	SafeRelease(m_impl);
}

// class ObjectQueue::InputEnd
ObjectQueue::InputEnd::InputEnd(ObjectQueue& queue)
	: m_impl(AddReference(queue.m_impl))
{
}

bool ObjectQueue::InputEnd::Push(Referencable* obj)
{
	unique_lock lock(m_impl->mutex_);
	return m_impl->Push(obj);
}

WaitResult ObjectQueue::InputEnd::Wait(int& code)
{
	unique_lock lock(m_impl->mutex_);
	return m_impl->WaitPush(lock, code);
}

WaitResult ObjectQueue::InputEnd::WaitFor(std::chrono::milliseconds timeout, int& code)
{
	unique_lock lock(m_impl->mutex_);
	return m_impl->WaitPushFor(lock, timeout, code);
}

WaitResult ObjectQueue::InputEnd::WaitPush(Referencable* obj, int& code)
{
	unique_lock lock(m_impl->mutex_);

	WaitResult result = m_impl->WaitPush(lock, code);
	_ASSERTE(result != WaitResult::Timeout);

	if (result == WaitResult::Ok)
	{
		bool br = m_impl->Push(obj);
		_ASSERTE(br);
	}

	return result;
}

WaitResult ObjectQueue::InputEnd::WaitPushFor(Referencable* obj, std::chrono::milliseconds timeout, int& code)
{
	unique_lock lock(m_impl->mutex_);

	WaitResult result = m_impl->WaitPushFor(lock, timeout, code);

	if (result == WaitResult::Ok)
	{
		bool br = m_impl->Push(obj);
		_ASSERTE(br);
	}

	return result;
}

void ObjectQueue::InputEnd::_Clear() noexcept
{
	SafeRelease(m_impl);
}

// class ObjectQueue::OutputEnd
ObjectQueue::OutputEnd::OutputEnd(ObjectQueue& queue)
	: m_impl(AddReference(queue.m_impl))
{
}

bool ObjectQueue::OutputEnd::Pop(Referencable*& obj)
{
	_ASSERTE(obj == nullptr);
	unique_lock lock(m_impl->mutex_);
	return m_impl->Pop(obj);
}

WaitResult ObjectQueue::OutputEnd::Wait(int& code)
{
	unique_lock lock(m_impl->mutex_);
	return m_impl->WaitPop(lock, code);
}

WaitResult ObjectQueue::OutputEnd::WaitFor(std::chrono::milliseconds timeout, int& code)
{
	unique_lock lock(m_impl->mutex_);
	return m_impl->WaitPopFor(lock, timeout, code);
}

WaitResult ObjectQueue::OutputEnd::WaitPop(Referencable*& obj, int& code)
{
	unique_lock lock(m_impl->mutex_);

	WaitResult result = m_impl->WaitPop(lock, code);
	_ASSERTE(result != WaitResult::Timeout);

	if (result == WaitResult::Ok)
	{
		bool br = m_impl->Pop(obj);
		_ASSERTE(br);
	}

	return result;
}

WaitResult ObjectQueue::OutputEnd::WaitPopFor(Referencable*& obj, std::chrono::milliseconds timeout, int& code)
{	
	unique_lock lock(m_impl->mutex_);

	WaitResult result = m_impl->WaitPop(lock, code);

	if (result == WaitResult::Ok)
	{
		bool br = m_impl->Pop(obj);
		_ASSERTE(br);
	}

	return result;
}

void ObjectQueue::OutputEnd::_Clear() noexcept
{
	SafeRelease(m_impl);
}

} // namespace Channels
} // namespace Leo
