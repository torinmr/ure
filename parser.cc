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

unique_ptr<Regex> Parser::parse_literal() {
  if (idx < pattern.size() && reserved.count(pattern[idx]) == 0) {
    unique_ptr<Regex> r = make_unique<Regex>(NType::Literal, pattern[idx]);
    if (debug) {
      cout << "Consumed " << pattern[idx] << endl;
    }
    idx++;
    return r;
  }

  return nullptr;
}

unique_ptr<Regex> Parser::parse_wildcard() {
  if (!consume('.')) return nullptr;
  return make_unique<Regex>(NType::Wildcard);
}

unique_ptr<Regex> Parser::parse_char() {
  unique_ptr<Regex> r = parse_wildcard();
  if (r != nullptr) return r;
  return parse_literal();
}

unique_ptr<Regex> Parser::parse_paren() {
  if (!consume('(')) return nullptr;
  unique_ptr<Regex> r = parse_alternate();
  if (!consume(')')) return nullptr;
  return r;
}

unique_ptr<Regex> Parser::parse_item() {
  unique_ptr<Regex> r = parse_paren();
  if (r == nullptr) {
    r = parse_char();
  }
  if (r == nullptr) return nullptr;

  if (consume('?')) {
    return make_unique<Regex>(NType::Question, move(r));
  } else if (consume('+')) {
    return make_unique<Regex>(NType::Plus, move(r));
  } else if (consume('*')) {
    return make_unique<Regex>(NType::Star, move(r));
  } else {
    return r;
  }
}

unique_ptr<Regex> Parser::parse_concat() {
  unique_ptr<Regex> r1 = parse_item();
  if (r1 == nullptr) return nullptr;

  unique_ptr<Regex> r2 = parse_concat();
  return make_unique<Regex>(NType::Concat, move(r1), move(r2));  // r2 possibly null
}

unique_ptr<Regex> Parser::parse_alternate() {
  unique_ptr<Regex> r1 = parse_concat();
  if (r1 == nullptr) return nullptr;
  if (!consume('|')) {
    return make_unique<Regex>(NType::Alternate, move(r1));
  }

  unique_ptr<Regex> r2 = parse_alternate();
  if (r2 == nullptr) return nullptr;
  return make_unique<Regex>(NType::Alternate, move(r1), move(r2));
}

unique_ptr<Regex> Parser::parse(const string& pattern_) {
  pattern = pattern_;
  idx = 0;
  return parse_alternate();
}

}  // namespace ure