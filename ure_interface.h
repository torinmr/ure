#ifndef URE_INTERFACE_H
#define URE_INTERFACE_H

#include <string>

namespace ure {

// Interface loosely based on Sanjay Ghemawat's PCRE interface: http://man.he.net/man3/pcrecpp
class Ure {
 public:
  virtual bool full_match(const std::string& text) const = 0;
  virtual bool partial_match(const std::string& text) const = 0;
  virtual bool parsing_failed() const = 0;
};

}  // namespace ure

#endif  // URE_INTERFACE_H