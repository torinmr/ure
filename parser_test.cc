#include <memory>

#include <gtest/gtest.h>

#include "parser.h"

using namespace std;
using namespace ure;

TEST(ParserTest, ValidParse) {
  Parser parser;
  vector<Instruction> re = parser.parse("a(bb*)+a|.?[ab]");
  vector<Instruction> expected = {
    Instruction::Split(9),
    Instruction::Literal('a'),
    Instruction::Literal('b'),
    Instruction::Split(3),
    Instruction::Literal('b'),
    Instruction::Jump(-2),
    Instruction::Split(-4),
    Instruction::Literal('a'),
    Instruction::Jump(4),
    Instruction::Split(2),
    Instruction::Wildcard('.'),
    Instruction::Class(CharacterClass(false, {'a', 'b'}, {})),
    Instruction::Match(),
  };
  ASSERT_EQ(expected, re);

  re = parser.parse(R"delim(\(\)\|\?\+\*\.\\\[\]\#)delim");
  expected = {
    Instruction::Literal('('),
    Instruction::Literal(')'),
    Instruction::Literal('|'),
    Instruction::Literal('?'),
    Instruction::Literal('+'),
    Instruction::Literal('*'),
    Instruction::Literal('.'),
    Instruction::Literal('\\'),
    Instruction::Literal('['),
    Instruction::Literal(']'),
    Instruction::Literal('#'),
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

TEST(ParserTest, CharacterClasses) {
  Parser parser;
  vector<Instruction> expected;
  // Basic functionality

  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {'a', 'b'}, {})),
    parser.parse("[ab]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(true, {'a', 'b'}, {})),
    parser.parse("[^ab]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {}, {{'a', 'z'}})),
    parser.parse("[a-z]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {}, {{'a', 'z'}, {'A', 'Z'}})),
    parser.parse("[a-zA-Z]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(true, {'a', ' ', '$'}, {{'A', 'Z'}, {'0', '9'}})),
    parser.parse("[^a A-Z$0-9]").at(0)
  );
  // Valid parses that are corner cases (see design_notes.md for details).
  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {}, {})),
    parser.parse("[]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(true, {}, {})),
    parser.parse("[^]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(true, {'^'}, {})),
    parser.parse("[^^]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {'-', 'a'}, {})),
    parser.parse("[-a]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {'a', '-'}, {})),
    parser.parse("[a-]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(true, {'-'}, {})),
    parser.parse("[^-]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(true, {'a', '-'}, {})),
    parser.parse("[^a-]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(true, {'-', 'a'}, {})),
    parser.parse("[^-a]").at(0)
  );

  // Escapes
  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {'^'}, {})),
    parser.parse("[\\^]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(true, {'^'}, {})),
    parser.parse("[^\\^]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {'a', '-', 'b'}, {})),
    parser.parse("[a\\-b]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {'-', 'a'}, {})),
    parser.parse("[\\-a]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {'a', '-'}, {})),
    parser.parse("[a\\-]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {'\\'}, {})),
    parser.parse("[\\\\]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {']'}, {})),
    parser.parse("[\\]]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {'['}, {})),
    parser.parse("[\\[]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {'.', '*', '?', '|', '+', '(', ')'}, {})),
    parser.parse("[.*?|+()]").at(0)  // These symbols don't need to be escaped inside character class
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {'.', '*', '?', '|', '+', '(', ')'}, {})),
    parser.parse("[\\.\\*\\?\\|\\+\\(\\)]").at(0)  // But they can be optionally
  );

  // Invalid parses
  vector<Instruction> empty;
  ASSERT_EQ(empty, parser.parse("[]ab]"));
  ASSERT_EQ(4, parser.error_info().idx);

  ASSERT_EQ(empty, parser.parse("[a--]"));
  ASSERT_EQ(0, parser.error_info().idx);

  ASSERT_EQ(empty, parser.parse("[\\]"));
  ASSERT_EQ(0, parser.error_info().idx);

  ASSERT_EQ(empty, parser.parse("[[]"));
  ASSERT_EQ(0, parser.error_info().idx);

  // Redundant classes. Not sure how I should handle this.
  // For now, I'll just let them be added. It's slightly inefficient but will still work correctly.
  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {'a', 'a'}, {})),
    parser.parse("[aa]").at(0)
  );
  EXPECT_EQ(
    Instruction::Class(CharacterClass(false, {}, {{'a', 'z'}, {'a', 'z'}})),
    parser.parse("[a-za-z]").at(0)
  );
}