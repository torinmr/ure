#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace ure {

enum class IType {
  Literal,
  Wildcard,
  Class,
  Jump,
  Split,
  Match,
};

const std::set<char> supported_built_in_classes = { 'd', 'D', 's', 'S', 'w', 'W' };

class CharacterClass {
  public:
   CharacterClass() {};
   CharacterClass(bool negated, std::vector<char> characters,
                  std::vector<std::pair<char, char>> ranges)
     : negated(negated), characters(characters), ranges(ranges) {}

   bool match(char c) const;

   bool operator==(const CharacterClass& other) const;
   std::string str() const;

   bool negated;
   std::vector<char> characters;
   std::vector<std::pair<char, char>> ranges;
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

  union {
    char c;
    std::ptrdiff_t offset;
    std::unique_ptr<const CharacterClass> cclass;
  };

  Instruction() {}
  Instruction(const Instruction& other);
  Instruction& operator=(const Instruction& other);
  ~Instruction();

  // Consume the character c.
  static Instruction Literal(char c);

  // Consume a single character matching the wildcard or built-in character class.
  static Instruction Wildcard(char c);

  // Consume a single character matching the provided character class.
  static Instruction Class(std::unique_ptr<CharacterClass> character_class);
  static Instruction Class(const CharacterClass& character_class);

  // Jump forward/backward in bytecode program by offset instructions.
  static Instruction Jump(std::ptrdiff_t offset);

  // Either jump to the offset, or continue on to the next instruction.
  static Instruction Split(std::ptrdiff_t offset);

  // Regular expression matched!
  static Instruction Match();

  bool match_wildcard(char c) const;

  std::string str() const;
  bool operator==(const Instruction& other) const;

  // Compiled instructions to match any string. Used for partial_match implementation.
  static const std::vector<Instruction> match_all;

 private:
  Instruction(std::unique_ptr<CharacterClass> character_class);
};

std::ostream& operator<<(std::ostream& os, const Instruction& inst);
std::ostream& operator<<(std::ostream& os, const std::vector<Instruction>& program);



}  // namespace ure

#endif  // INSTRUCTION_H