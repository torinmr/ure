#include <cstddef>
#include <iostream>

#include "ure_nfa.h"

namespace ure {

using namespace std;

UreNfa::UreNfa(const string& pattern) {
  re = parser.parse(pattern);
  if (!re.empty()) {
    partial_re = Instruction::dot_star;
    for (const Instruction& inst : re) {
      partial_re.push_back(inst);
    }
  }
}

bool UreNfa::full_match(const string& text) const {
  return false;
}

bool UreNfa::partial_match(const string& text) const {
  return false;
}

bool UreNfa::parsing_failed() const { return re.empty(); }
ParseError UreNfa::parser_error_info() { return parser.error_info(); }

}  // namespace ure