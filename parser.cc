#include <iostream>
#include <set>
#include "parser.h"

namespace ure {

using namespace std;

// Characters that can't be a normal unescaped literal.
set<char> reserved = { '(', ')', '|', '?', '+', '*', '.' };

bool Parser::consume(char c) {
  if (idx < pattern.size() && pattern[idx] == c) {
    idx++;
    if (debug) {
      cout << "Consumed " << c << endl;
    }
    return true;
  }
  return false;
}

Regex* Parser::parse_literal() {
  if (idx < pattern.size() && reserved.count(pattern[idx]) == 0) {
    Regex* r = new Regex(NType::Literal, pattern[idx]);
    if (debug) {
      cout << "Consumed " << pattern[idx] << endl;
    }
    idx++;
    return r;
  }

  return nullptr;
}

Regex* Parser::parse_wildcard() {
  if (!consume('.')) return nullptr;
  return new Regex(NType::Wildcard);
}

Regex* Parser::parse_char() {
  Regex* r = parse_wildcard();
  if (r != nullptr) return r;
  return parse_literal();
}

Regex* Parser::parse_paren() {
  if (!consume('(')) return nullptr;
  Regex* r = parse_alternate();
  if (!consume(')')) return nullptr;
  return r;
}

Regex* Parser::parse_item() {
  Regex* r = parse_paren();
  if (r == nullptr) {
    r = parse_char();
  }
  if (r == nullptr) return nullptr;

  if (consume('?')) {
    return new Regex(NType::Question, r);
  } else if (consume('+')) {
    return new Regex(NType::Plus, r);
  } else if (consume('*')) {
    return new Regex(NType::Star, r);
  } else {
    return r;
  }
}

Regex* Parser::parse_concat() {
  Regex* r1 = parse_item();
  if (r1 == nullptr) return nullptr;

  Regex* r2 = parse_concat();
  return new Regex(NType::Concat, r1, r2);  // r2 possibly null
}

Regex* Parser::parse_alternate() {
  Regex* r1 = parse_concat();
  if (r1 == nullptr) return nullptr;
  if (!consume('|')) {
    return new Regex(NType::Alternate, r1);
  }

  Regex* r2 = parse_alternate();
  if (r2 == nullptr) return nullptr;
  return new Regex(NType::Alternate, r1, r2);
}

Regex* Parser::parse(const string& pattern_) {
  pattern = pattern_;
  idx = 0;
  return parse_alternate();
}

}  // namespace ure