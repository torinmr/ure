#ifndef URE_RECURSIVE_H
#define URE_RECURSIVE_H

#include <memory>
#include <vector>

#include "parser.h"
#include "ure_interface.h"

namespace ure {

struct Regex;

class UreRecursive : public Ure {
 public:
  UreRecursive(const std::string& pattern);
  bool FullMatch(const std::string& text) override;
  bool PartialMatch(const std::string& text) override;

  bool parsing_failed();
  ParseError parser_error_info();

 private:
  std::vector<Instruction> re;
  Parser parser;
};

}  // namespace ure

#endif  // URE_RECURSIVE_H