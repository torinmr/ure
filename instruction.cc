#include <cctype>
#include <iostream>
#include <string>

#include "instruction.h"

namespace ure {

using namespace std;

bool CharacterClass::match(char c) const {
  for (char expected : characters) {
    if (c == expected) return !negated;
  }
  for (const auto& range : ranges) {
    if (range.first <= c && c <= range.second) return !negated;
  }
  return negated;
}

bool CharacterClass::operator==(const CharacterClass& other) const {
  return negated == other.negated && characters == other.characters && ranges == other.ranges;
}

string CharacterClass::str() const {
  string s = "";
  if (negated) {
    s += "^";
  }
  for (char c : characters) {
    s += c;
  }
  for (const auto& range : ranges) {
    s += range.first;
    s += '-';
    s += range.second;
  }

  return s;
}

Instruction::Instruction(const Instruction& other) : type(other.type) {
  switch (type) {
    case IType::Literal:  // fallthrough
    case IType::Wildcard:
      c = other.c;
      break;
    case IType::Jump:  // fallthrough
    case IType::Split:
      offset = other.offset;
      break;
    case IType::Class:
      new (&cclass) unique_ptr<const CharacterClass>(
        make_unique<CharacterClass>(*other.cclass)
      );
      break;
    case IType::Match:  // fallthrough
    default:
      break;
  }
}

Instruction& Instruction::operator=(const Instruction& other) {
  if (type == IType::Class) {
    cclass.~unique_ptr<const CharacterClass>();
  }
  type = other.type;
  switch (type) {
    case IType::Literal:  // fallthrough
    case IType::Wildcard:
      c = other.c;
      break;
    case IType::Jump:  // fallthrough
    case IType::Split:
      offset = other.offset;
      break;
    case IType::Class:
      cclass = make_unique<CharacterClass>(*other.cclass);
      break;
    case IType::Match:  // fallthrough
    default:
      break;
  }

  return *this;
}

Instruction::Instruction(unique_ptr<CharacterClass> character_class) {
  new (&cclass) unique_ptr<const CharacterClass>(move(character_class));
  type = IType::Class;
}

Instruction::~Instruction() {
  if (type == IType::Class) {
    cclass.~unique_ptr<const CharacterClass>();
  }
}

Instruction Instruction::Literal(char c) {
  Instruction inst;
  inst.type = IType::Literal;
  inst.c = c;
  return inst;
}

Instruction Instruction::Wildcard(char c) {
  assert(supported_built_in_classes.count(c) == 1 || c == '.' || c == '*');
  Instruction inst;
  inst.type = IType::Wildcard;
  inst.c = c;
  return inst;
}

Instruction Instruction::Class(unique_ptr<CharacterClass> character_class) {
  return Instruction(move(character_class));
}

Instruction Instruction::Class(const CharacterClass& character_class) {
  return Instruction(make_unique<CharacterClass>(character_class));
}

Instruction Instruction::Jump(std::ptrdiff_t offset) {
  Instruction inst;
  inst.type = IType::Jump;
  inst.offset = offset;
  return inst;
}

Instruction Instruction::Split(std::ptrdiff_t offset) {
  Instruction inst;
  inst.type = IType::Split;
  inst.offset = offset;
  return inst;
}

Instruction Instruction::Match() {
  Instruction inst;
  inst.type = IType::Match;
  return inst;
}

string type_to_str(IType t) {
  switch (t) {
    case IType::Literal: return "Literal";
    case IType::Wildcard: return "Wildcard";
    case IType::Jump: return "Jump";
    case IType::Split: return "Split";
    case IType::Match: return "Match";
    case IType::Class: return "Class";
    default: return "Unknown instruction type";
  }
}

string Instruction::str() const {
  string s = type_to_str(type);
  switch (type) {
    case IType::Literal: return s + " " + c;
    case IType::Wildcard: return s + " " + c;
    case IType::Jump: return s + " " + to_string(offset);
    case IType::Split: return s + " " + to_string(offset);
    case IType::Match: return s;
    case IType::Class: return s + " " + cclass->str();
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
    case IType::Literal: return c == other.c;
    case IType::Wildcard: return true;
    case IType::Jump: return offset == other.offset;
    case IType::Split: return offset == other.offset;
    case IType::Match: return true;
    case IType::Class: return *cclass == *other.cclass;
    default:
      cerr << "Unknown type" << endl;
      return false;
  }
}

bool Instruction::match_wildcard(char input) const {
  assert(type == IType::Wildcard);
  switch (c) {
    // Not usable for user-provided regular expressions: only used in match_all defined below.
    case '*': return true;
    case '.': return input != '\n' && input != '\r';
    case 'd': return isdigit(input);
    case 'D': return !isdigit(input);
    case 's': return isspace(input);
    case 'S': return !isspace(input);
    case 'w': return isalnum(input) || input == '_';
    case 'W': return !isalnum(input) && input != '_';
    default:
      cerr << "Unknown wildcard " << c << endl;
      return false;
  }
}

const vector<Instruction> Instruction::match_all{
  Split(3), Wildcard('*'), Jump(-2)
};

}  // namespace ure