#include <iostream>
#include <set>
#include "parser.h"

namespace ure {

using namespace std;

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

// Recursive descent parser that builds up a bytecode program based on the regular
// expression. Each parse_* function handles an element in the EBNF grammar (described
// in parser.h), trying to parse characters beginning at pattern[idx].
// If parsing succeeds, it appends instructions onto the end of progam.
// If parsing fails, it restore program and idx to their initial state.

bool Parser::parse_literal(vector<Instruction>& program) {
  if (idx < pattern.size() && reserved.count(pattern[idx]) == 0) {
    program.push_back(Instruction::Literal(pattern[idx]));
    if (debug) {
      cout << "Consumed " << pattern[idx] << endl;
    }
    idx++;
    return true;
  }

  return false;
}

bool Parser::parse_wildcard(vector<Instruction>& program) {
  if (!consume('.')) return false;
  program.push_back(Instruction::Wildcard());
  return true;
}

bool Parser::parse_char(vector<Instruction>& program) {
  return parse_wildcard(program) || parse_literal(program);
}

bool Parser::parse_paren(vector<Instruction>& program) {
  size_t initial_idx = idx;
  size_t initial_pc = program.size();
  if (!consume('(')) return false;
  parse_alternate(program);
  if (!consume(')')) {
    idx = initial_idx;
    program.resize(initial_pc);
    return false;
  }
  return true;
}

bool Parser::parse_item(vector<Instruction>& program) {
  ptrdiff_t initial_pc = program.size();
  bool parsed = parse_paren(program) || parse_char(program);
  if (!parsed) return false;

  if (consume('?')) {
    // a? compiles to:
    //  0 Split 2
    //  1 Literal a
    //  2 ...
    program.insert(program.begin() + initial_pc,
                   Instruction::Split(program.size() + 1 - initial_pc));
  } else if (consume('+')) {
    // a+ compiles to:
    //  0 Literal a
    //  1 Split -1
    //  2 ...
    program.push_back(Instruction::Split(initial_pc - program.size()));
  } else if (consume('*')) {
    // a* compiles to:
    //   0 Split 3
    //   1 Literal a
    //   2 Jmp -2
    //   3 ...
    program.insert(program.begin() + initial_pc,
                   Instruction::Split(program.size() + 2 - initial_pc));
    program.push_back(Instruction::Jump(initial_pc - program.size()));
  }

  return true;
}

bool Parser::parse_concat(vector<Instruction>& program) {
  if (!parse_item(program)) return false;
  parse_concat(program);
  return true;
}

// Given regex like "a|b", produces code like:
//
//   0 Split 3
//   1 Literal a
//   2 Jmp 2
//   3 Literal b
//   4 (rest of regex)
//
// In the terminal case where the alternate contains only one option, avoids
// producing any extra bytecode.
bool Parser::parse_alternate(vector<Instruction>& program) {
  ptrdiff_t initial_pc = program.size();

  // We don't check the return value of the recursive calls, because we can
  // always parse to an empty regex.
  parse_concat(program);
  if (!consume('|')) {
    // "Alternate" is just a single clause, no special instructions needed.
    return true;
  }

  // Now that we know there are two alternatives, splice a split instruction
  // back into the program. Inserting in the middle of a vector is inefficient,
  // but it should be fine unless regexes are extremely long.
  program.insert(program.begin() + initial_pc,
                 Instruction::Split(program.size() + 2 - initial_pc));

  // Add the jump instruction. We need to parse what comes next before filling
  // in the value of the jump offset.
  ptrdiff_t jmp_pc = program.size();
  program.push_back(Instruction::Jump(0));
  parse_alternate(program);
  program[jmp_pc].offset = program.size() - jmp_pc;
  return true;
}

vector<Instruction> Parser::parse(const string& pattern_) {
  pattern = pattern_;
  idx = 0;
  vector<Instruction> program;
  if (!parse_alternate(program)) return {};
  if (idx < pattern.size()) return {};
  program.push_back(Instruction::Match());

  if (debug) {
    cout << program << endl;
  }
  return program;
}

ParseError Parser::error_info() {
  return { .pattern = pattern, .idx = idx };
}

}  // namespace ure