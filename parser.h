#ifndef PARSER_H
#define PARSER_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "instruction.h"

namespace ure {

// An error occurred while parsing at pattern[idx], see msg for details.
struct ParseError {
  std::string pattern;
  std::size_t idx;
};

// Recursive-descent parser for regular expressions. Compiles parsed regular expression
// to a bytecode program, see instruction.h.
//
// Implements the following EBNF grammar:
//
// Alternate            = Concat | Empty, [ "|", Alternate ]
// Concat               = Item, [Concat]
// Item                 = Paren | Char | Question | Plus | Star
// Paren                = "(", Alternate, ")"
// Question             = (Paren | Char), "?"
// Plus                 = (Paren | Char), "+"
// Star                 = (Paren | Char), "*"
// Char                 = Wildcard | Literal | Escape | Class
// Wildcard             = "."
// Literal              = "a" | "b" | ... (not ReservedLiteral)
// Escape               = "\", (NonAlphaNum | BuiltInClass)
// ReservedLiteral      = "." | "(" | "\" | ...
// NonAlphaNum          = (all characters except a-zA-Z0-9)
// Empty                = ""
// Class                = "[", (NegatedClass | Class) "]"
// NegatedClass         = "^", ClassContents
// ClassContents        = ["-"], [ClassElements], ["-"]
// ClassElements        = ClassElement, [ClassElements]
// ClassElement         = ClassRange | ClassChar
// ClassRange           = ClassChar, "-", ClassChar
// ClassChar            = ClassEscape | ClassLiteral
// ClassEscape          = "\", NonAlphaNum
// ClassLiteral         = "a" | "b" | ... (not "]", "\", "-")
// BuiltInClass         = "d" | "s" | ...

// Grammar currently supports:
//   Classical regex operators: |, *, concatenation
//   Parentheses ()
//   ? and +
//   . as a wildcard
//   Escapes for reserved characters: \., \\, \?, etc.
//   Predefined character classes: \d, \D, \w, \W, \s, \S
//   User-defined character classes ([a-z], [^@], etc.)
//
// Future work:
//   Anchors (^, $)
class Parser {
 public:
  Parser(bool debug = false) : debug(debug) {}

  // Attempt to parse the regular expression.
  // If successful, returns a vector of instructions.
  // If unsuccessful, returns an empty vector. (Valid patterns can never produce
  // an empty instruction vector, as it will at least have a Match instruction.)
  std::vector<Instruction> parse(const std::string& pattern);

  // Access information about parse errors (only valid if parse() returned empty vector).
  ParseError error_info();

 private:
  std::string pattern;
  std::size_t idx;
  bool debug;

  bool consume(char c);
  bool parse_alternate(std::vector<Instruction>& program);
  bool parse_concat(std::vector<Instruction>& program);
  bool parse_item(std::vector<Instruction>& program);
  bool parse_paren(std::vector<Instruction>& program);
  bool parse_char(std::vector<Instruction>& program);
  bool parse_wildcard(std::vector<Instruction>& program);
  bool parse_literal(std::vector<Instruction>& program);
  bool parse_escape(std::vector<Instruction>& program);
  bool parse_class(std::vector<Instruction>& program);
  bool parse_class_element(CharacterClass& cclass);
  bool parse_class_char(char& c);
  bool parse_class_literal(char& c);
  bool parse_class_escape(char& c);
};

}  // namespace ure

#endif  // PARSER_H