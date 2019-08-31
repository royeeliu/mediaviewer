#include "Threading.Channels.ObjectQueue.h"
#include <list>
#include <mutex>
#include <condition_variable>

using namespace std::chrono;

namespace Leo { namespace Threading { namespace Channels {

using unique_lock = std::unique_lock<std::mutex>;

struct ObjectQueue::Impl : Referencable
{
	uint32_t								maxSize_ = 1;
	std::list<ReferenceGuard<Referencable>> objects_;
	std::list<int>							sendingInterrupts_;
	std::list<int>							receivingInterrupts_;
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
		sendingInterrupts_.push_back(code);
		receivingInterrupts_.push_back(code);
		cv_.notify_one();
	}

	void InterruptSendingEnd(int code)
	{
		sendingInterrupts_.push_back(code);
		cv_.notify_one();
	}

	void InterruptReceivintEnd(int code)
	{
		receivingInterrupts_.push_back(code);
		cv_.notify_one();
	}

	bool TryPush(Referencable* obj)
	{
		if (objects_.size() >= maxSize_ || !sendingInterrupts_.empty())
		{
			return false;
		}

		objects_.push_back(ReferenceGuard<Referencable>::Attach(AddReference(obj)));
		cv_.notify_one();
		return true;
	}

	bool TryPop(Referencable*& obj)
	{
		if (objects_.empty() || !receivingInterrupts_.empty())
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

	Status WaitPush(unique_lock& lock, int& code)
	{
		while (true)
		{
			if (!sendingInterrupts_.empty())
			{
				code = sendingInterrupts_.front();
				sendingInterrupts_.pop_front();
				return Status::Interrupted;
			}

			if (objects_.size() < maxSize_)
			{
				return Status::Ok;
			}

			cv_.wait(lock);
		}
	}

	Status WaitPushFor(unique_lock& lock, milliseconds timeout, int& code)
	{
		auto startTime = high_resolution_clock::now();

		while (true)
		{
			if (!sendingInterrupts_.empty())
			{
				code = sendingInterrupts_.front();
				sendingInterrupts_.pop_front();
				return Status::Interrupted;
			}

			if (objects_.size() < maxSize_)
			{
				return Status::Ok;
			}

			auto elapsed = duration_cast<milliseconds>(high_resolution_clock::now() - startTime);

			if (timeout <= elapsed)
			{
				return Status::Timeout;
			}

			std::cv_status status = cv_.wait_for(lock, timeout - elapsed);

			if (status == std::cv_status::timeout)
			{
				return Status::Timeout;
			}
		}
	}

	Status WaitPop(unique_lock& lock, int& code)
	{
		while (true)
		{
			if (!receivingInterrupts_.empty())
			{
				code = receivingInterrupts_.front();
				receivingInterrupts_.pop_front();
				return Status::Interrupted;
			}

			if (!objects_.empty())
			{
				return Status::Ok;
			}

			cv_.wait(lock);
		}
	}

	Status WaitPopFor(unique_lock& lock, milliseconds timeout, int& code)
	{
		auto startTime = high_resolution_clock::now();

		while (true)
		{
			if (!receivingInterrupts_.empty())
			{
				code = receivingInterrupts_.front();
				receivingInterrupts_.pop_front();
				return Status::Interrupted;
			}

			if (!objects_.empty())
			{
				return Status::Ok;
			}

			auto elapsed = duration_cast<milliseconds>(high_resolution_clock::now() - startTime);

			if (timeout <= elapsed)
			{
				return Status::Timeout;
			}

			std::cv_status status = cv_.wait_for(lock, timeout - elapsed);

			if (status == std::cv_status::timeout)
			{
				return Status::Timeout;
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

void ObjectQueue::InterruptSendingEnd(int code) noexcept
{
	unique_lock lock(m_impl->mutex_);
	m_impl->InterruptSendingEnd(code);
}

void ObjectQueue::InterruptReceivingEnd(int code) noexcept
{
	unique_lock lock(m_impl->mutex_);
	m_impl->InterruptReceivintEnd(code);
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
ObjectQueue::SendingEnd::SendingEnd(ObjectQueue& queue)
	: m_impl(AddReference(queue.m_impl))
{
}

bool ObjectQueue::SendingEnd::TrySend(Referencable* obj)
{
	unique_lock lock(m_impl->mutex_);
	return m_impl->TryPush(obj);
}

ObjectQueue::Status ObjectQueue::SendingEnd::Wait(int& code)
{
	unique_lock lock(m_impl->mutex_);
	return m_impl->WaitPush(lock, code);
}

ObjectQueue::Status ObjectQueue::SendingEnd::WaitFor(std::chrono::milliseconds timeout, int& code)
{
	unique_lock lock(m_impl->mutex_);
	return m_impl->WaitPushFor(lock, timeout, code);
}

ObjectQueue::Status ObjectQueue::SendingEnd::Send(Referencable* obj, int& code)
{
	unique_lock lock(m_impl->mutex_);

	Status result = m_impl->WaitPush(lock, code);
	_ASSERTE(result != Status::Timeout);

	if (result == Status::Ok)
	{
		bool br = m_impl->TryPush(obj);
		_ASSERTE(br);
	}

	return result;
}

ObjectQueue::Status ObjectQueue::SendingEnd::SendFor(Referencable* obj, std::chrono::milliseconds timeout, int& code)
{
	unique_lock lock(m_impl->mutex_);

	Status result = m_impl->WaitPushFor(lock, timeout, code);

	if (result == Status::Ok)
	{
		bool br = m_impl->TryPush(obj);
		_ASSERTE(br);
	}

	return result;
}

void ObjectQueue::SendingEnd::_Clear() noexcept
{
	SafeRelease(m_impl);
}

// class ObjectQueue::OutputEnd
ObjectQueue::ReceivingEnd::ReceivingEnd(ObjectQueue& queue)
	: m_impl(AddReference(queue.m_impl))
{
}

bool ObjectQueue::ReceivingEnd::TryReceive(Referencable*& obj)
{
	_ASSERTE(obj == nullptr);
	unique_lock lock(m_impl->mutex_);
	return m_impl->TryPop(obj);
}

ObjectQueue::Status ObjectQueue::ReceivingEnd::Wait(int& code)
{
	unique_lock lock(m_impl->mutex_);
	return m_impl->WaitPop(lock, code);
}

ObjectQueue::Status ObjectQueue::ReceivingEnd::WaitFor(std::chrono::milliseconds timeout, int& code)
{
	unique_lock lock(m_impl->mutex_);
	return m_impl->WaitPopFor(lock, timeout, code);
}

ObjectQueue::Status ObjectQueue::ReceivingEnd::Receive(Referencable*& obj, int& code)
{
	unique_lock lock(m_impl->mutex_);

	Status result = m_impl->WaitPop(lock, code);
	_ASSERTE(result != Status::Timeout);

	if (result == Status::Ok)
	{
		bool br = m_impl->TryPop(obj);
		_ASSERTE(br);
	}

	return result;
}

ObjectQueue::Status ObjectQueue::ReceivingEnd::ReceiveFor(Referencable*& obj, std::chrono::milliseconds timeout, int& code)
{	
	unique_lock lock(m_impl->mutex_);

	Status result = m_impl->WaitPop(lock, code);

	if (result == Status::Ok)
	{
		bool br = m_impl->TryPop(obj);
		_ASSERTE(br);
	}

	return result;
}

void ObjectQueue::ReceivingEnd::_Clear() noexcept
{
	SafeRelease(m_impl);
}

} // namespace Channels
} // namespace Threading
} // namespace Leo
