#include <gtest/gtest.h>
#include "ure_stl.h"

using namespace ure;

TEST(UreTest, TestStl) {
  UreStl ure("a(bb)+a");
  EXPECT_TRUE(ure.FullMatch("abbbba"));
  EXPECT_FALSE(ure.FullMatch("abbba"));
  EXPECT_FALSE(ure.FullMatch("zzzabbbbazzz"));
  EXPECT_TRUE(ure.PartialMatch("zzzabbbbazzz"));
  EXPECT_FALSE(ure.PartialMatch("zzzabbbazzz"));
}