#ifndef URE_RECURSIVE_H
#define URE_RECURSIVE_H

#include <cstddef>
#include <memory>
#include <vector>

#include "parser.h"
#include "ure_interface.h"

namespace ure {

struct Regex;

// No attempt has been made to make this implementation thread-safe.
class UreRecursive : public Ure {
 public:
  UreRecursive(const std::string& pattern);
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

#endif  // URE_RECURSIVE_H