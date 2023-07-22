#include <iostream>

#include "ure_recursive.h"

namespace ure {

using namespace std;

UreRecursive::UreRecursive(const string& pattern) {
  re = parser.parse(pattern);
}

bool UreRecursive::full_match(const string& text) const {
  return false;
}

bool UreRecursive::partial_match(const string& text) const {
 return false;
}

bool UreRecursive::parsing_failed() const { return re.empty(); }
ParseError UreRecursive::parser_error_info() { return parser.error_info(); }

}  // namespace ure