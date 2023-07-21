#include <iostream>

#include "ure_recursive.h"

namespace ure {

using namespace std;

UreRecursive::UreRecursive(const string& pattern) {
  re = parser.parse(pattern);
}

bool UreRecursive::FullMatch(const string& text) {
  return false;
}

bool UreRecursive::PartialMatch(const string& text) {
 return false;
}

bool UreRecursive::parsing_failed() { return re == nullptr; }
ParseError UreRecursive::parser_error_info() { return parser.error_info(); }

}  // namespace ure