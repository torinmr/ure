#include <string>

#include "regex.h"

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

void indent(ostream& os, int depth) {
  for (int i = 0; i < depth; i++) {
    os << " ";
  }
}

ostream& output_regex(ostream& os, const Regex& r, int depth) {
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

ostream& operator<<(ostream& os, const Regex& r) {
  return output_regex(os, r, 0);
}

}  // namespace ure