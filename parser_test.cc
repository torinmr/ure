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
  ASSERT_EQ(expected, re);

  re = parser.parse(R"delim(\(\)\|\?\+\*\.\\)delim");
  expected = {
    Instruction::Literal('('),
    Instruction::Literal(')'),
    Instruction::Literal('|'),
    Instruction::Literal('?'),
    Instruction::Literal('+'),
    Instruction::Literal('*'),
    Instruction::Literal('.'),
    Instruction::Literal('\\'),
    Instruction::Match(),
  };
  ASSERT_EQ(expected, re);
}

TEST(ParserTest, EmptyParse) {
  Parser parser;

  vector<Instruction> expected = {
    Instruction::Match()
  };
  ASSERT_EQ(expected, parser.parse(""));
  ASSERT_EQ(expected, parser.parse("()"));

  expected = {
    Instruction::Split(2),
    Instruction::Jump(1),
    Instruction::Match()
  };
  ASSERT_EQ(expected, parser.parse("|"));

  expected = {
    Instruction::Split(2),
    Instruction::Jump(-1),
    Instruction::Match()
  };
  ASSERT_EQ(expected, parser.parse("()*"));
}

TEST(ParserTest, InvalidParse) {
  Parser parser;
  vector<Instruction> empty;

  ASSERT_EQ(empty, parser.parse("abc??"));
  ASSERT_EQ(4, parser.error_info().idx);
  ASSERT_EQ("abc??", parser.error_info().pattern);

  ASSERT_EQ(empty, parser.parse("abc("));
  ASSERT_EQ(3, parser.error_info().idx);

  ASSERT_EQ(empty, parser.parse("("));
  ASSERT_EQ(0, parser.error_info().idx);

  ASSERT_EQ(empty, parser.parse("*abc"));
  ASSERT_EQ(0, parser.error_info().idx);

  ASSERT_EQ(empty, parser.parse("(a)b)"));
  ASSERT_EQ(4, parser.error_info().idx);

  ASSERT_EQ(empty, parser.parse("ab\\a"));
  ASSERT_EQ(2, parser.error_info().idx);

  // Actually valid - any non-alphanumeric character can be escaped.
  ASSERT_NE(empty, parser.parse("ab\\#"));
}