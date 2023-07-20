#ifndef URE_STL_H
#define URE_STL_H

#include <regex>
#include "ure_interface.h"

namespace ure {

// An implementation which simply calls the built-in STL regex library.
// Intended to be used for testing other implementations.
class UreStl : public Ure {
 public:
  UreStl(const std::string& pattern) : re(pattern) {}

  bool FullMatch(const std::string& text) override {
    return std::regex_match(text, re);
  }

  bool PartialMatch(const std::string& text) override {
    return std::regex_search(text, re);
  }

 private:
  std::regex re;
};

}  // namespace ure

#endif  // URE_STL_H