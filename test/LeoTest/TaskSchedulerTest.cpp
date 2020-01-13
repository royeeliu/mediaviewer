#include "pch.h"
#include "Leo/Tasks/Scheduler.h"
#include <functional>
#include <future>

using namespace Leo::Tasks;

class DefaultSchedulerTest : public testing::Test
{
public:
	virtual void SetUp() override
	{
	}

	virtual void TearDown() override
	{
		// 恢复默认值避免影响后续调用
		SetDefaultScheduler(std::shared_ptr<SchedulerInterface>{});
	}
};

namespace {

struct TaskWrapper
{
	std::promise<int> prom;
	static const int result = 100;

	static void TaskProc(void* param)
	{
		static_cast<TaskWrapper*>(param)->prom.set_value(result);
	}
};

struct CustomScheduler : SchedulerInterface
{
	virtual void Schedule(TaskProc_t proc, void* param) override
	{
		proc(param);
	}
};

} // end of anonymous namespace

TEST_F(DefaultSchedulerTest, GetDefaultScheduler)
{
	auto scheduler = GetDefaultScheduler();
	ASSERT_NE(scheduler, nullptr);
}

TEST_F(DefaultSchedulerTest, SetDefaultSchedulerWithNull)
{
	SetDefaultScheduler(std::shared_ptr<SchedulerInterface>{});
	auto scheduler = GetDefaultScheduler();
	ASSERT_NE(scheduler, nullptr);
	SetDefaultScheduler(std::shared_ptr<SchedulerInterface>{});
	scheduler = GetDefaultScheduler();
	ASSERT_NE(scheduler, nullptr);
}

TEST_F(DefaultSchedulerTest, SetDefaultSchedulerWithCustomScheduler)
{
	auto scheduler = std::make_shared<CustomScheduler>();
	SetDefaultScheduler(scheduler);
	auto defaultScheduler = GetDefaultScheduler();
	EXPECT_EQ(defaultScheduler, scheduler);
}

TEST_F(DefaultSchedulerTest, ScheduleTask)
{
	auto scheduler = GetDefaultScheduler();
	ASSERT_NE(scheduler, nullptr);
	TaskWrapper task;
	scheduler->Schedule(&TaskWrapper::TaskProc, &task);
	auto fut = task.prom.get_future();
	auto status = fut.wait_for(std::chrono::seconds{ 10 });
	ASSERT_EQ(status, std::future_status::ready);
	int result = fut.get();
	ASSERT_EQ(result, task.result);
}
