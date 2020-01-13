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
