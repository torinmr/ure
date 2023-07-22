#ifndef URE_STL_H
#define URE_STL_H

#include <regex>
#include "ure_interface.h"

namespace ure {

// An implementation which simply calls the built-in STL regex library.
// Intended to be used for testing other implementations.
class UreStl : public Ure {
 public:
  UreStl(const std::string& pattern) {
    try {
      re = std::regex(pattern);
      parsed = true;
    } catch (const std::regex_error& e) {
      parsed = false;
    }
  }

  bool full_match(const std::string& text) const override {
    return parsed && std::regex_match(text, re);
  }

  bool partial_match(const std::string& text) const override {
    return parsed && std::regex_search(text, re);
  }

  bool parsing_failed() const { return !parsed; }

 private:
  bool parsed;
  std::regex re;
};

}  // namespace ure

#endif  // URE_STL_H