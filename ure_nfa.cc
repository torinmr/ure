#include <cstddef>
#include <iostream>
#include <set>

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

struct Thread {
  size_t pc;

  Thread(size_t pc) : pc(pc) {}
};

struct ThreadList {
  ThreadList(size_t program_size) : used(program_size, false) {
    threads.reserve(program_size);
  };

  void add(size_t pc) {
    if (used[pc]) return;
    threads.emplace_back(pc);
    used[pc] = true;
  }

  size_t size() const {
    return threads.size();
  }

  Thread& operator[](size_t idx) {
    return threads[idx];
  }

  const Thread& operator[](size_t idx) const {
    return threads[idx];
  }

 private:
  vector<Thread> threads;
  vector<bool> used;
};

bool match(const vector<Instruction>& program, const string& text, bool partial = false) {
  ThreadList threads(program.size());
  threads.add({0});
  for (size_t idx = 0; idx <= text.size(); idx++) {
    ThreadList next_threads(program.size());
    for (size_t t = 0; t < threads.size(); t++) {
      size_t pc = threads[t].pc;
      if (threads[t].pc >= program.size()) {
        cerr << "Invalid program counter " << pc
             << ", program.size() is " << program.size() << endl;
        return false;
      }

      const Instruction& inst = program[pc];
      switch (inst.type) {
        case IType::Literal:
          if (idx < text.size() && inst.c == text[idx]) {
            next_threads.add(pc+1);
          }
          break;
        case IType::Wildcard:
          if (idx < text.size()) {
            next_threads.add(pc+1);
          }
          break;
        case IType::Jump:
          threads.add(pc + inst.offset);
          break;
        case IType::Split:
          threads.add(pc+1);
          threads.add(pc + inst.offset);
          break;
        case IType::Match:
          if (partial || idx == text.size()) return true;
          break;
        default:
          cerr << "Unknown instruction type" << endl;
          return false;
      }
    }
    threads = move(next_threads);
  }
  return false;
}

bool UreNfa::full_match(const string& text) const {
  return match(re, text);
}

bool UreNfa::partial_match(const string& text) const {
  return match(partial_re, text, true);
}

bool UreNfa::parsing_failed() const { return re.empty(); }
ParseError UreNfa::parser_error_info() { return parser.error_info(); }

}  // namespace ure