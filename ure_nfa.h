#ifndef URE_NFA_H
#define URE_NFA_H

#include <cstddef>
#include <memory>
#include <vector>

#include "parser.h"
#include "ure_interface.h"

namespace ure {

struct Regex;

// No attempt has been made to make this implementation thread-safe.
class UreNfa : public Ure {
 public:
  UreNfa(const std::string& pattern);
  bool full_match(const std::string& text) const override;
  bool partial_match(const std::string& text) const override;

  bool parsing_failed() const override;
  ParseError parser_error_info();

 private:
  std::vector<Instruction> re;
  std::vector<Instruction> partial_re;
  Parser parser;
};

}  // namespace ure

#endif  // URE_NFA_H