#include <iostream>

#include "ure_recursive.h"

namespace ure {

using namespace std;

UreRecursive::UreRecursive(const string& pattern) {
  Parser parser;
  re = parser.parse(pattern);
}

bool UreRecursive::FullMatch(const string& text) { return false; }
bool UreRecursive::PartialMatch(const string& text) { return false; }

}  // namespace ure