#ifndef URE_INTERFACE_H
#define URE_INTERFACE_H

#include <string>

namespace ure {

// Interface loosely based on Sanjay Ghemawat's PCRE interface: http://man.he.net/man3/pcrecpp
class Ure {
 public:
  virtual bool FullMatch(const std::string& text) = 0;
  virtual bool PartialMatch(const std::string& text) = 0;
};

}  // namespace ure

#endif  // URE_INTERFACE_H