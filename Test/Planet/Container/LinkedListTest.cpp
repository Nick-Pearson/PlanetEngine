#include "gtest/gtest.h"

TEST(HelloTest, GetGreet) {
  EXPECT_STREQ("Hello Bazel", "Hello Bazel");
}