#include "pch.h"
#include "Leo/Error.h"

using namespace Leo;

TEST(ErrorTest, CreateWithoutErrorCode)
{
    auto err = Error{};
    EXPECT_FALSE(err);
    EXPECT_EQ(err.Code(), 0);
}

TEST(ErrorTest, CreateWithErrorCode_0)
{
    auto err = Error{ 0 };
    EXPECT_FALSE(err);
    EXPECT_EQ(err.Code(), 0);
}

TEST(ErrorTest, CreateWithErrorCode_1)
{
    auto err = Error{ 1 };
    EXPECT_TRUE(err);
    EXPECT_EQ(err.Code(), 1);
}

TEST(ErrorTest, CreateWithErrorCode_Minus_1)
{
    auto err = Error{ -1 };
    EXPECT_TRUE(err);
    EXPECT_EQ(err.Code(), -1);
}

TEST(ErrorTest, TestMoveConstructor)
{
    auto err1 = Error{ 1 };
    auto err2 = Error{ std::move(err1) };
    EXPECT_FALSE(err1);
    EXPECT_TRUE(err2);
    EXPECT_EQ(err1.Code(), 0);
    EXPECT_EQ(err2.Code(), 1);
}

TEST(ErrorTest, TestMoveAssign)
{
	auto err1 = Error{ 1 };
	auto err2 = Error{ 0 };
    err2 = std::move(err1);
	EXPECT_FALSE(err1);
	EXPECT_TRUE(err2);
	EXPECT_EQ(err1.Code(), 0);
	EXPECT_EQ(err2.Code(), 1);
}

TEST(ErrorTest, TestSelfMoveAssign)
{
	auto err = Error{ 1 };
    err = std::move(err);
	EXPECT_TRUE(err);
	EXPECT_EQ(err.Code(), 1);
}

TEST(ErrorTest, ClearError)
{
	auto err = Error{ 1 };
    err.Clear();
	EXPECT_FALSE(err);
	EXPECT_EQ(err.Code(), 0);
}
