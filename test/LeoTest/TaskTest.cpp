#include "pch.h"
#include "Leo/Tasks.h"

using namespace Leo::Tasks;

TEST(TaskTest, ConstructEmptyTask)
{
	Task<void> void_task;
	EXPECT_FALSE(void_task);
	Task<int> int_task;
	EXPECT_FALSE(int_task);
}

TEST(TaskTest, ConstructIntTask)
{
	Task<int> task{ [] { return 42; } };
	auto result = task.Wait();
	EXPECT_TRUE(result.Succeeded());
	EXPECT_EQ(result.Get(), 42);
}

TEST(TaskTest, ConstructTaskWithFunctionReturnResult)
{
	Task<int> task{ [] { return Leo::Result<int>::MakeSucceededResult(42); } };
	auto result = task.Wait();
	EXPECT_TRUE(result.Succeeded());
	EXPECT_EQ(result.Get(), 42);
}

TEST(TaskTest, ConstructTaskWithFunctionReturnFailureResult)
{
	Task<int> task{ [] { return Leo::Result<int>::MakeFailureResult(Leo::Error{ -1 }); } };
	auto result = task.Wait();
	EXPECT_TRUE(result.Failed());
	EXPECT_EQ(result.Error().Code(), -1);
}
