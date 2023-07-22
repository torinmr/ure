#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstddef>
#include <iostream>
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
  char c;
  std::ptrdiff_t offset;

  // Consume the character c.
  static Instruction Literal(char c) {
    return { IType::Literal, c };
  }

  // Consume any single character ("." wildcard).
  static Instruction Wildcard() {
    return { IType::Wildcard };
  }

  // Jump forward/backward in bytecode program by offset instructions.
  static Instruction Jump(std::ptrdiff_t offset) {
    return { IType::Jump, 0, offset };
  }

  // Either jump to the offset, or continue on to the next instruction.
  static Instruction Split(std::ptrdiff_t offset) {
    return { IType::Split, 0, offset };
  }

  // Regular expression matched!
  static Instruction Match() {
    return { IType::Match };
  }

  std::string str() const;
  bool operator==(const Instruction& other) const;

  // Compiled instructions for ".*".
  static const std::vector<Instruction> dot_star;
};

std::ostream& operator<<(std::ostream& os, const Instruction& inst);
std::ostream& operator<<(std::ostream& os, const std::vector<Instruction>& program);



}  // namespace ure

#endif  // INSTRUCTION_H