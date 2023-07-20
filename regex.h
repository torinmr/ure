#ifndef REGEX_H
#define REGEX_H

#include <iostream>
#include <memory>

namespace ure {

enum class NType {
  Alternate,
  Concat,
  Question,
  Plus,
  Star,
  Wildcard,
  Literal,
  Empty,
};

// Datatype used for parsed regular expressions.
struct Regex {
  NType type;
  char c = 0;  // Only used for Literal
  std::unique_ptr<Regex> r1 {};
  std::unique_ptr<Regex> r2 {};  // Only used for Alternate/Concat

  Regex(NType type) : type(type) {}
  Regex(NType type, char c)
    : type(type), c(c) {}
  Regex(NType type, std::unique_ptr<Regex> r1)
    : type(type), r1(move(r1)) {}
  Regex(NType type, std::unique_ptr<Regex> r1, std::unique_ptr<Regex> r2)
    : type(type), r1(move(r1)), r2(move(r2)) {}

  bool operator==(const Regex& other) const;
  bool operator!=(const Regex& other) const;
};

std::ostream& operator<<(std::ostream&, const Regex&);

}  // namespace ure

#endif  // REGEX_H