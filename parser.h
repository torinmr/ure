#ifndef PARSER_H
#define PARSER_H

#include <cstddef>
#include <memory>
#include <string>

#include "regex.h"

// Recursive-descent parser for regular expressions. Implements the following EBNF grammar:
//
// Alternate = Concat, [ "|", Alternate ]
// Concat    = item, [Concat]
// item      = paren | char | Question | Plus | Star
// paren     = "(", Alternate, ")"
// Question  = (paren | char), "?"
// Plus      = (paren | char), "+"
// Star      = (paren | char), "*"
// char      = Wildcard | Literal
// Wildcard  = "."
// Literal   = "a" | "b" | ...
//
// Capitalized items are part of the final parsed form, while lowercase items
// are only used as part of the parsing process.
//
// Grammar currently supports:
//   Classical regex operators: |, *, concatenation
//   Parentheses ()
//   ? and +
//   . as a wildcard

namespace ure {

// An error occurred while parsing at pattern[idx], see msg for details.
struct ParseError {
  std::string pattern;
  std::size_t idx;
};

class Parser {
 public:
  Parser(bool debug = false) : debug(debug) {}

  // Attempt to parse the regular expression.
  // If successful, return a pointer to a Regex struct.
  // If unsuccessful, return nullptr and populate error information.
  std::unique_ptr<Regex> parse(const std::string& pattern);

  // Access information about parse errors (only valid if parse() returned nullptr).
  ParseError error_info();

 private:
  std::string pattern;
  std::size_t idx;
  bool debug;

  bool consume(char c);
  std::unique_ptr<Regex> parse_alternate();
  std::unique_ptr<Regex> parse_concat();
  std::unique_ptr<Regex> parse_item();
  std::unique_ptr<Regex> parse_paren();
  std::unique_ptr<Regex> parse_char();
  std::unique_ptr<Regex> parse_wildcard();
  std::unique_ptr<Regex> parse_literal();
};

}  // namespace ure

#endif  // PARSER_H