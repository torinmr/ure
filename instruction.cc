#include <iostream>
#include <string>

#include "instruction.h"

namespace ure {

using namespace std;

string type_to_str(IType t) {
  switch (t) {
    case IType::Literal: return "Literal";
    case IType::Wildcard: return "Wildcard";
    case IType::Jump: return "Jump";
    case IType::Split: return "Split";
    case IType::Match: return "Match";
    default: return "Unknown instruction type";
  }
}

string Instruction::str() const {
  string s = type_to_str(type);
  switch (type) {
    case IType::Literal: return s + " " + c;
    case IType::Wildcard: return s;
    case IType::Jump: return s + " " + to_string(offset);
    case IType::Split: return s + " " + to_string(offset);
    case IType::Match: return s;
    default: return "Unknown instruction type";
  }
}

ostream& operator<<(ostream& os, const Instruction& inst) {
  return os << inst.str();
}

ostream& operator<<(ostream& os, const vector<Instruction>& program) {
  for (size_t i = 0; i < program.size(); i++) {
    os << i << " " << program[i] << endl;
  }
  return os;
}

bool Instruction::operator==(const Instruction& other) const {
  return type == other.type && c == other.c && offset == other.offset;
}

// Compiled instructions for ".*".
const vector<Instruction> dot_star {
  Instruction::Split(3),
  Instruction::Wildcard(),
  Instruction::Jump(-2)
};

vector<Instruction> Instruction::to_partial(const vector<Instruction>& program) {
  if (program.empty()) return program;

  vector<Instruction> full_program = dot_star;
  for (const Instruction& inst : program) {
    full_program.push_back(inst);
  }
  full_program.insert(full_program.end() - 1, dot_star.begin(), dot_star.end());
  return full_program;
}

}  // namespace ure