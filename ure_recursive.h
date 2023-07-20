#ifndef URE_RECURSIVE_H
#define URE_RECURSIVE_H

#include "parser.h"
#include "ure_interface.h"

namespace ure {

struct Regex;

class UreRecursive : public Ure {
 public:
  UreRecursive(const std::string& pattern);
  bool FullMatch(const std::string& text) override;
  bool PartialMatch(const std::string& text) override;

 private:
  Regex* re;
};

}  // namespace ure

#endif  // URE_RECURSIVE_H