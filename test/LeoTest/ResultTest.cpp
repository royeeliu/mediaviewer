#include "pch.h"
#include "Leo/Result.h"

using namespace Leo;

TEST(ResultDeathTest, GetResultFromIntFailureResult)
{
	auto result = Result<int>::MakeFailureResult(1);
	ASSERT_DEATH(result.Get(), "");
}

TEST(ResultDeathTest, GetResultFromVoidFailureResult)
{
	auto result = Result<void>::MakeFailureResult(1);
	ASSERT_DEATH(result.Get(), "");
}

TEST(ResultDeathTest, GetResultFromIntCanceledResult)
{
	auto result = Result<int>::MakeCanceledResult();
	ASSERT_DEATH(result.Get(), "");
}

TEST(ResultDeathTest, GetResultFromVoidCaneceledResult)
{
	auto result = Result<void>::MakeCanceledResult();
	ASSERT_DEATH(result.Get(), "");
}

TEST(ResultTest, CreateVoidFailureResultWithErrorCode)
{
	auto result = Result<void>::MakeFailureResult(1);
	EXPECT_EQ(result.Status(), ResultStatus::Failed);
	EXPECT_FALSE(result);
	EXPECT_FALSE(result.Succeeded());
	EXPECT_TRUE(result.Failed());
	EXPECT_FALSE(result.Canceled());

	auto err = result.Error();
	EXPECT_EQ(err.Code(), 1);
}

TEST(ResultTest, CreateIntFailureResultWithErrorCode)
{
	auto result = Result<int>::MakeFailureResult(1);
	EXPECT_EQ(result.Status(), ResultStatus::Failed);
	EXPECT_FALSE(result);
	EXPECT_FALSE(result.Succeeded());
	EXPECT_TRUE(result.Failed());
	EXPECT_FALSE(result.Canceled());

	auto err = result.Error();
	EXPECT_EQ(err.Code(), 1);
}

TEST(ResultTest, CreateVoidFailureResultWithError)
{
	auto err = Error{ 1 };
	auto result = Result<void>::MakeFailureResult(std::move(err));
	EXPECT_EQ(result.Status(), ResultStatus::Failed);
	EXPECT_EQ(err.Code(), 0);
	EXPECT_FALSE(result);
	EXPECT_FALSE(result.Succeeded());
	EXPECT_TRUE(result.Failed());
	EXPECT_FALSE(result.Canceled());

	err = result.Error();
	EXPECT_EQ(err.Code(), 1);
}

TEST(ResultTest, CreateIntFailureResultWithError)
{
	auto err = Error{ 1 };
	auto result = Result<int>::MakeFailureResult(std::move(err));
	EXPECT_EQ(result.Status(), ResultStatus::Failed);
	EXPECT_FALSE(result);
	EXPECT_FALSE(result.Succeeded());
	EXPECT_TRUE(result.Failed());
	EXPECT_FALSE(result.Canceled());

	auto err1 = result.Error();
	EXPECT_EQ(err1.Code(), 1);
}

TEST(ResultTest, CreateVoidCanceledResult)
{
	auto result = Result<void>::MakeCanceledResult();
	EXPECT_EQ(result.Status(), ResultStatus::Canceled);
	EXPECT_FALSE(result);
	EXPECT_FALSE(result.Succeeded());
	EXPECT_FALSE(result.Failed());
	EXPECT_TRUE(result.Canceled());

	auto err = result.Error();
	EXPECT_EQ(err.Code(), 0);
}

TEST(ResultTest, CreateIntCanceledResult)
{
	auto result = Result<int>::MakeCanceledResult();
	EXPECT_EQ(result.Status(), ResultStatus::Canceled);
	EXPECT_FALSE(result);
	EXPECT_FALSE(result.Succeeded());
	EXPECT_FALSE(result.Failed());
	EXPECT_TRUE(result.Canceled());

	auto err = result.Error();
	EXPECT_EQ(err.Code(), 0);
}

TEST(ResultTest, CreateVoidSucceededResult)
{
	auto result = Result<void>::MakeSucceededResult();
	auto err = result.Error();
	ASSERT_EQ(result.Status(), ResultStatus::Succeeded);
	result.Get();
	EXPECT_TRUE(result);
	EXPECT_TRUE(result.Succeeded());
	EXPECT_FALSE(result.Failed());
	EXPECT_FALSE(result.Canceled());
	EXPECT_EQ(err.Code(), 0);
}

TEST(ResultTest, CreateIntSucceededResult)
{
	auto result = Result<int>::MakeSucceededResult(100);
	auto err = result.Error();
	ASSERT_EQ(result.Status(), ResultStatus::Succeeded);
	EXPECT_EQ(result.Get(), 100);
	EXPECT_TRUE(result);
	EXPECT_TRUE(result.Succeeded());
	EXPECT_FALSE(result.Failed());
	EXPECT_FALSE(result.Canceled());
	EXPECT_EQ(err.Code(), 0);
}

TEST(ResultTest, CreateStringSucceededResult)
{
	std::string str = "test string";
	auto result = Result<std::string>::MakeSucceededResult(str);
	auto err = result.Error();
	ASSERT_EQ(result.Status(), ResultStatus::Succeeded);
	EXPECT_EQ(result.Get(), str);
	EXPECT_TRUE(result);
	EXPECT_TRUE(result.Succeeded());
	EXPECT_FALSE(result.Failed());
	EXPECT_FALSE(result.Canceled());
	EXPECT_EQ(err.Code(), 0);
}

TEST(ResultTest, CopyConstructFailureResult)
{
	auto result1 = Result<int>::MakeFailureResult(1);
	auto result2 = result1;
	auto err1 = result1.Error();
	auto err2 = result2.Error();
	EXPECT_EQ(result1.Status(), ResultStatus::Failed);
	EXPECT_EQ(result2.Status(), ResultStatus::Failed);
	EXPECT_EQ(err1.Code(), 1);
	EXPECT_EQ(err2.Code(), 1);
}

TEST(ResultTest, CopyConstructCanceledResult)
{
	auto result1 = Result<int>::MakeCanceledResult();
	auto result2 = result1;
	auto err1 = result1.Error();
	auto err2 = result2.Error();
	EXPECT_EQ(result1.Status(), ResultStatus::Canceled);
	EXPECT_EQ(result2.Status(), ResultStatus::Canceled);
	EXPECT_EQ(err1.Code(), 0);
	EXPECT_EQ(err2.Code(), 0);
}

TEST(ResultTest, CopyConstructVoidSucceededResult)
{
	auto result1 = Result<void>::MakeSucceededResult();
	auto result2 = result1;
	ASSERT_EQ(result1.Status(), ResultStatus::Succeeded);
	ASSERT_EQ(result2.Status(), ResultStatus::Succeeded);
}

TEST(ResultTest, CopyConstructIntSucceededResult)
{
	auto result1 = Result<int>::MakeSucceededResult(100);
	auto result2 = result1;
	ASSERT_EQ(result1.Status(), ResultStatus::Succeeded);
	ASSERT_EQ(result2.Status(), ResultStatus::Succeeded);
	EXPECT_EQ(result1.Get(), 100);
	EXPECT_EQ(result2.Get(), 100);
}

TEST(ResultTest, CopyIntFailureResult)
{
	auto result1 = Result<int>::MakeCanceledResult();
	auto result2 = Result<int>::MakeFailureResult(1);
	result1 = result2;
	auto err1 = result1.Error();
	auto err2 = result2.Error();
	EXPECT_EQ(result1.Status(), ResultStatus::Failed);
	EXPECT_EQ(result2.Status(), ResultStatus::Failed);
	EXPECT_EQ(err1.Code(), 1);
	EXPECT_EQ(err2.Code(), 1);
}

TEST(ResultTest, CopyIntCanceledResult)
{
	auto result1 = Result<int>::MakeFailureResult(1);
	auto result2 = Result<int>::MakeCanceledResult();
	result1 = result2;
	auto err1 = result1.Error();
	auto err2 = result2.Error();
	EXPECT_EQ(result1.Status(), ResultStatus::Canceled);
	EXPECT_EQ(result2.Status(), ResultStatus::Canceled);
	EXPECT_EQ(err1.Code(), 0);
	EXPECT_EQ(err2.Code(), 0);
}

TEST(ResultTest, CopyVoidFailureResult)
{
	auto result1 = Result<void>::MakeCanceledResult();
	auto result2 = Result<void>::MakeFailureResult(1);
	result1 = result2;
	auto err1 = result1.Error();
	auto err2 = result2.Error();
	EXPECT_EQ(result1.Status(), ResultStatus::Failed);
	EXPECT_EQ(result2.Status(), ResultStatus::Failed);
	EXPECT_EQ(err1.Code(), 1);
	EXPECT_EQ(err2.Code(), 1);
}

TEST(ResultTest, CopyVoidCanceledResult)
{
	auto result1 = Result<void>::MakeFailureResult(1);
	auto result2 = Result<void>::MakeCanceledResult();
	result1 = result2;
	auto err1 = result1.Error();
	auto err2 = result2.Error();
	EXPECT_EQ(result1.Status(), ResultStatus::Canceled);
	EXPECT_EQ(result2.Status(), ResultStatus::Canceled);
	EXPECT_EQ(err1.Code(), 0);
	EXPECT_EQ(err2.Code(), 0);
}

TEST(ResultTest, CopyVoidSucceededResult)
{
	auto result1 = Result<void>::MakeFailureResult(1);
	auto result2 = Result<void>::MakeSucceededResult();
	result1 = result2;
	auto err1 = result1.Error();
	auto err2 = result2.Error();
	EXPECT_EQ(result1.Status(), ResultStatus::Succeeded);
	EXPECT_EQ(result2.Status(), ResultStatus::Succeeded);
	EXPECT_EQ(err1.Code(), 0);
	EXPECT_EQ(err2.Code(), 0);
}

TEST(ResultTest, CopyIntSucceededResult)
{
	auto result1 = Result<int>::MakeFailureResult(1);
	auto result2 = Result<int>::MakeSucceededResult(100);
	result1 = result2;
	auto err1 = result1.Error();
	auto err2 = result2.Error();
	ASSERT_EQ(result1.Status(), ResultStatus::Succeeded);
	ASSERT_EQ(result2.Status(), ResultStatus::Succeeded);
	ASSERT_EQ(result1.Get(), 100);
	ASSERT_EQ(result2.Get(), 100);
	EXPECT_EQ(err1.Code(), 0);
	EXPECT_EQ(err2.Code(), 0);
}
