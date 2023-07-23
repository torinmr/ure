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
    case IType::Literal: return s + " " + arg.c;
    case IType::Wildcard: return s;
    case IType::Jump: return s + " " + to_string(arg.offset);
    case IType::Split: return s + " " + to_string(arg.offset);
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
  if (type != other.type) return false;
  switch (type) {
    case IType::Literal: return arg.c == other.arg.c;
    case IType::Wildcard: return true;
    case IType::Jump: return arg.offset == other.arg.offset;
    case IType::Split: return arg.offset == other.arg.offset;
    case IType::Match: return true;
    default:
      cout << "Unknown type" << endl;
      return false;
  }
}

const vector<Instruction> Instruction::dot_star{
  Split(3), Wildcard(), Jump(-2)
};

}  // namespace ure