#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::AllOf;
using ::testing::HasSubstr;
using ::testing::ThrowsMessage;

int foo(int x) { return x + 5; }

namespace {
TEST(FooTest, Check) { EXPECT_EQ(foo(4), (int)(4 + 5)); }
}  // namespace
