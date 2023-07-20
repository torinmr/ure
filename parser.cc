#include <iostream>
#include <set>
#include "parser.h"

namespace ure {

using namespace std;

bool Regex::operator==(const Regex& other) const {
  if (type != other.type) return false;
  if (c != other.c) return false;
  if (r1 != nullptr && other.r1 != nullptr) {
    if (*r1 != *other.r1) return false;
  } else {
    if (r1 != other.r1) return false;
  }
  if (r2 != nullptr && other.r2 != nullptr) {
    if (*r2 != *other.r2) return false;
  } else {
    if (r2 != other.r2) return false;
  }
  return true;
}

bool Regex::operator!=(const Regex& other) const {
  return !(*this == other);
}

string type_to_str(NType t) {
  switch (t) {
    case NType::Alternate: return "Alternate";
    case NType::Concat: return "Concat";
    case NType::Question: return "Question";
    case NType::Plus: return "Plus";
    case NType::Star: return "Star";
    case NType::Wildcard: return "Wildcard";
    case NType::Literal: return "Literal";
    default: return "Unknown type";
  }
}

void indent(std::ostream& os, int depth) {
  for (int i = 0; i < depth; i++) {
    os << " ";
  }
}

std::ostream& output_regex(std::ostream& os, const Regex& r, int depth) {
  indent(os, depth);
  os << "Regex(" << type_to_str(r.type);
  if (r.c) {
    os << ", " << r.c << ")";
    return os;
  }
  if (r.r1 == nullptr && r.r2 == nullptr) {
    os << ")";
    return os;
  }

  os << "," << endl;
  int new_depth = depth + 2;
  if (r.r1 != nullptr) {
    output_regex(os, *r.r1, new_depth);
    os << endl;
  }
  if (r.r2 != nullptr) {
    output_regex(os, *r.r2, new_depth);
    os << endl;
  }
  indent(os, depth);
  return os << ")";
}

std::ostream& operator<<(std::ostream& os, const Regex& r) {
  return output_regex(os, r, 0);
}

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