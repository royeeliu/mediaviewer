﻿#include "pch.h"
#include "Leo/Threading/Task.h"

using namespace Leo::Threading;

TEST(TaskTest, CreateEmptyTask)
{
	Task<void> void_task;
	EXPECT_FALSE(void_task);
	Task<int> int_task;
	EXPECT_FALSE(int_task);
}