#ifndef PARSER_H
#define PARSER_H

#include <cstddef>
#include <memory>
#include <string>

#include "regex.h"

namespace ure {
// EBNF Grammar used for the recursive-descent parser:
//
// Capitalized items are part of the final parsed form, while lowercase items
// are only used as part of the parsing process.
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
// Grammar currently supports:
//   Classical regex operators: |, *, concatenation
//   Parentheses ()
//   ? and +
//   . as a wildcard

class Parser {
 public:
  Parser(bool debug = false) : debug(debug) {}
  std::unique_ptr<Regex> parse(const std::string& pattern);

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