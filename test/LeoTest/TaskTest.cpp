#include "pch.h"
#include "Leo/Tasks.h"

using namespace Leo::Tasks;

TEST(TaskTest, CreateEmptyTask)
{
	Task<void> void_task;
	EXPECT_FALSE(void_task);
	Task<int> int_task;
	EXPECT_FALSE(int_task);
}

TEST(TaskTest, CreatIntTask)
{
	Task<int> task{ [] { return 42; } };
	auto result = task.Wait();
	EXPECT_TRUE(result.Succeeded());
	EXPECT_EQ(result.Get(), 42);
}
