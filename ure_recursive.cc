#include <cassert>
#include <cstddef>
#include <iostream>

#include "ure_recursive.h"

namespace ure {

using namespace std;

UreRecursive::UreRecursive(const string& pattern) {
  re = parser.parse(pattern);
  partial_re = Instruction::to_partial(re);
}

bool match(const vector<Instruction>& program, const string& text,
           vector<vector<bool>>& visited, size_t pc, size_t idx) {
  if (pc >= program.size()) {
    cerr << "Invalid program counter " << pc << ", program.size() is " << program.size() << endl;
    return false;
  }
  if (idx > text.size()) {
    cerr << "Invalid idx " << idx << ", text.size() is " << text.size() << endl;
    return false;
  }

  if (visited[pc][idx]) return false;  // Infinite loop
  visited[pc][idx] = true;

  const Instruction& inst = program[pc];
  switch (inst.type) {
    case IType::Literal:
      if (idx < text.size() && inst.c == text[idx]) {
        return match(program, text, visited, pc + 1, idx + 1);
      }
      return false;
    case IType::Wildcard:
      if (idx < text.size()) {
        return match(program, text, visited, pc + 1, idx + 1);
      }
      return false;
    case IType::Jump:
      return match(program, text, visited, pc + inst.offset, idx);
    case IType::Split:
      return match(program, text, visited, pc + 1, idx) ||
             match(program, text, visited, pc + inst.offset, idx);
    case IType::Match:
      return idx == text.size();
    default:
      cerr << "Unknown instruction type" << endl;
      return false;
  }
}

bool UreRecursive::full_match(const string& text) const {
  vector<vector<bool>> visited(re.size(),
      vector<bool>(text.size() + 1));
  return match(re, text, visited, 0, 0);
}

bool UreRecursive::partial_match(const string& text) const {
  vector<vector<bool>> visited(partial_re.size(),
      vector<bool>(text.size() + 1));
  return match(partial_re, text, visited, 0, 0);
}

bool UreRecursive::parsing_failed() const { return re.empty(); }
ParseError UreRecursive::parser_error_info() { return parser.error_info(); }

}  // namespace ure