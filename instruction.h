#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cassert>
#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace ure {

enum class IType {
  Literal,
  Wildcard,
  Jump,
  Split,
  Match,
};

const std::set<char> supported_built_in_classes = { 'd', 'D', 's', 'S', 'w', 'W' };

// Bytecode for compiled regular expressions. Each regular expression compiles to a
// vector of Instructions (see parser.h). For example:
//  "ab*" -> { Literal('a'), Split(3), Literal('b'), Jump(-2), Match() }
//
// Based on Russ Cox's article "Regular Expression Matching: the Virtual Machine Approach":
// https://swtch.com/~rsc/regexp/regexp2.html. I've made some modifications, like using offsets
// in the instruction array instead of pointers and simplifying the bytecode for the Split
// Instruction.
struct Instruction {
  IType type;

  union Argument {
    char c;
    std::ptrdiff_t offset;
  };
  Argument arg;

  // Consume the character c.
  static Instruction Literal(char c) {
    Instruction inst;
    inst.type = IType::Literal;
    inst.arg.c = c;
    return inst;
  }

  // Consume a single character matching the wildcard or built-in character class.
  static Instruction Wildcard(char c) {
    assert(supported_built_in_classes.count(c) == 1 || c == '.' || c == '*');
    Instruction inst;
    inst.type = IType::Wildcard;
    inst.arg.c = c;
    return inst;
  }

  // Jump forward/backward in bytecode program by offset instructions.
  static Instruction Jump(std::ptrdiff_t offset) {
    Instruction inst;
    inst.type = IType::Jump;
    inst.arg.offset = offset;
    return inst;
  }

  // Either jump to the offset, or continue on to the next instruction.
  static Instruction Split(std::ptrdiff_t offset) {
    Instruction inst;
    inst.type = IType::Split;
    inst.arg.offset = offset;
    return inst;
  }

  // Regular expression matched!
  static Instruction Match() {
    return { IType::Match };
  }

  bool match_wildcard(char c) const;

  std::string str() const;
  bool operator==(const Instruction& other) const;

  // Compiled instructions to match any string. Used for partial_match implementation.
  static const std::vector<Instruction> match_all;
};

std::ostream& operator<<(std::ostream& os, const Instruction& inst);
std::ostream& operator<<(std::ostream& os, const std::vector<Instruction>& program);



}  // namespace ure

#endif  // INSTRUCTION_H