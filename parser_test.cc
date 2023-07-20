#include <gtest/gtest.h>
#include "parser.h"

using namespace ure;

TEST(ParserTest, BasicTest) {
  Parser parser(true);
  parser.parse("a(bb)+a");
}