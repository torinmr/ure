#include <memory>

#include <gtest/gtest.h>

#include "parser.h"

using namespace std;
using namespace ure;

TEST(ParserTest, ValidParse) {
  Parser parser;
  vector<Instruction> re = parser.parse("a(bb*)+a|.?");
  vector<Instruction> expected = {
    Instruction::Split(9),
    Instruction::Literal('a'),
    Instruction::Literal('b'),
    Instruction::Split(3),
    Instruction::Literal('b'),
    Instruction::Jump(-2),
    Instruction::Split(-4),
    Instruction::Literal('a'),
    Instruction::Jump(3),
    Instruction::Split(2),
    Instruction::Wildcard(),
    Instruction::Match(),
  };
  ASSERT_EQ(re, expected);
}

TEST(ParserTest, EmptyParse) {
  Parser parser;

  vector<Instruction> expected = {
    Instruction::Match()
  };
  ASSERT_EQ(parser.parse(""), expected);
  ASSERT_EQ(parser.parse("()"), expected);

  expected = {
    Instruction::Split(2),
    Instruction::Jump(1),
    Instruction::Match()
  };
  ASSERT_EQ(parser.parse("|"), expected);

  expected = {
    Instruction::Split(2),
    Instruction::Jump(-1),
    Instruction::Match()
  };
  ASSERT_EQ(parser.parse("()*"), expected);
}

TEST(ParserTest, InvalidParse) {
  Parser parser;
  vector<Instruction> empty;

  ASSERT_EQ(parser.parse("abc??"), empty);
  ASSERT_EQ(parser.error_info().idx, 4);
  ASSERT_EQ(parser.error_info().pattern, "abc??");

  ASSERT_EQ(parser.parse("abc("), empty);
  ASSERT_EQ(parser.error_info().idx, 3);

  ASSERT_EQ(parser.parse("("), empty);
  ASSERT_EQ(parser.error_info().idx, 0);

  ASSERT_EQ(parser.parse("*abc"), empty);
  ASSERT_EQ(parser.error_info().idx, 0);

  ASSERT_EQ(parser.parse("(a)b)"), empty);
  ASSERT_EQ(parser.error_info().idx, 4);
}