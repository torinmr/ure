#ifndef PARSER_H
#define PARSER_H

#include <cstddef>
#include <memory>
#include <string>

namespace ure {
// EBNF Grammar used for the recursive-descent parser.
//
// Grammar currently supports:
//   Classical regex operators: |, *, concatenation
//   Parentheses ()
//   ? and +
//   . as a wildcard
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

enum class NType {
  Alternate,
  Concat,
  Question,
  Plus,
  Star,
  Wildcard,
  Literal,
};

struct Regex {
  NType type;
  char c;  // Only used for Literal
  std::unique_ptr<Regex> r1;
  std::unique_ptr<Regex> r2;  // Only used for Alternate/Concat

  Regex(NType type) : type(type) {}
  Regex(NType type, char c)
    : type(type), c(c) {}
  Regex(NType type, std::unique_ptr<Regex> r1)
    : type(type), r1(move(r1)) {}
  Regex(NType type, std::unique_ptr<Regex> r1, std::unique_ptr<Regex> r2)
    : type(type), r1(move(r1)), r2(move(r2)) {}
};

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