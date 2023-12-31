#include <iostream>
#include <limits>
#include <string>

#include <gtest/gtest.h>

#include "ure_nfa.h"
#include "ure_recursive.h"
#include "ure_stl.h"

using namespace ure;
using namespace std;

TEST(UreTest, TestStl) {
  UreStl ure("a(bb)+a");
  EXPECT_TRUE(ure.full_match("abbbba"));
  EXPECT_FALSE(ure.full_match("abbba"));
  EXPECT_FALSE(ure.full_match("zzzabbbbazzz"));
  EXPECT_TRUE(ure.partial_match("zzzabbbbazzz"));
  EXPECT_FALSE(ure.partial_match("zzzabbbazzz"));
}

int pow(int a, int b) {
  int result = 1;
  for (int i = 0; i < b; i++) {
    result *= a;
  }
  return result;
}

// Sequences our regex engine doesn't yet support.
vector<string> forbidden_sequences{
  // std::regex parser considers two adjacent quantifiers to be valid.
  // The ones suffixed with ? are lazy quantifiers, which aren't implemented yet.
  // But I'm not sure why the rest are allowed.
  "**",
  "*?",
  "*+",
  "+*",
  "+?",
  "++",
  "?*",
  "??",
  "?+",
  // Character classes we don't support.
  "\\a",
  "\\b",
  "\\c",
};

void test_all_patterns(const Ure& reference_re, const Ure& test_re,
                       const string& pattern, const string& text_chars, int max_length) {
  size_t num_chars = text_chars.size();
  for (int length = 0; length <= max_length; length++) {
    string text(length, ' ');
    int num_texts = pow(num_chars, length);
    for (int text_idx = 0; text_idx < num_texts; text_idx++) {
      for (int i = 0; i < length; i++) {
        text[length - i - 1] = text_chars[(text_idx / pow(num_chars, i)) % num_chars];
      }

      EXPECT_EQ(reference_re.full_match(text), test_re.full_match(text))
          << "Pattern: \"" << pattern << "\", Text: \"" << text << "\"";
      EXPECT_EQ(reference_re.partial_match(text), test_re.partial_match(text))
          << "Pattern: \"" << pattern << "\", Text: \"" << text << "\"";
    }
  }
}

template<typename Reference, typename Test>
void test_all_regexes(const string& re_chars, int max_re_length,
                      const string& text_chars, int max_text_length,
                      bool parse_only = false) {
  size_t num_chars = re_chars.size();
  for (int length = 0; length <= max_re_length; length++) {
    string pattern(length, ' ');
    int num_regexes = pow(num_chars, length);
    for (int re_idx = 0; re_idx < num_regexes; re_idx++) {
      for (int i = 0; i < length; i++) {
        pattern[length - i - 1] = re_chars[(re_idx / pow(num_chars, i)) % num_chars];
      }
      bool valid = true;
      for (const string& seq: forbidden_sequences) {
        if (pattern.find(seq) != string::npos) {
          valid = false;
          break;
        }
      }
      if (!valid) continue;
      Reference reference_re(pattern);
      Test test_re(pattern);

      EXPECT_EQ(reference_re.parsing_failed(), test_re.parsing_failed())
          << "Pattern: \"" << pattern << "\"";

      if (parse_only || reference_re.parsing_failed() || test_re.parsing_failed()) continue;
      test_all_patterns(reference_re, test_re, pattern, text_chars, max_text_length);
    }
  }
}

// Test a character class by comparing it to every (ASCII) char.
template<typename Reference, typename Test>
void test_class(const string& pattern) {
  Reference reference_re(pattern);
  Test test_re(pattern);
  EXPECT_EQ(reference_re.parsing_failed(), test_re.parsing_failed())
      << "Pattern: \"" << pattern << "\"";
  if (reference_re.parsing_failed() || test_re.parsing_failed()) return;

  for (int c = 0; c < 128; c++) {
    string text(1, c);
    EXPECT_EQ(reference_re.full_match(text), test_re.full_match(text))
        << "Pattern: \"" << pattern << "\", Text: \"" << text << "\"" << " (char " << c << ")";
  }
}

TEST(UreTest, TestRecursive) {
  UreRecursive ure("a(bb)+a");
  ASSERT_FALSE(ure.parsing_failed());
  ASSERT_TRUE(ure.full_match("abbbba"));
  ASSERT_FALSE(ure.full_match("abbba"));
  ASSERT_FALSE(ure.full_match("zzzabbbbazzz"));
  ASSERT_TRUE(ure.partial_match("zzzabbbbazzz"));
  ASSERT_FALSE(ure.partial_match("zzzabbbazzz"));

  UreRecursive bad("a(b");
  ASSERT_TRUE(bad.parsing_failed());
  ASSERT_EQ(1, bad.parser_error_info().idx);

  ASSERT_TRUE(UreRecursive("abc").partial_match("\nabc\n"));

  test_class<UreStl, UreRecursive>(".");
  test_class<UreStl, UreNfa>("\\d");
  test_class<UreStl, UreNfa>("\\D");
  test_class<UreStl, UreNfa>("\\s");
  test_class<UreStl, UreNfa>("\\S");
  test_class<UreStl, UreNfa>("\\w");
  test_class<UreStl, UreNfa>("\\W");

  test_class<UreStl, UreNfa>("[ab]");
  test_class<UreStl, UreNfa>("[^ab]");
  test_class<UreStl, UreNfa>("[a-z]");
  test_class<UreStl, UreNfa>("[a-zA-Z]");
  test_class<UreStl, UreNfa>("[^a A-Z$0-9]");
  test_class<UreStl, UreNfa>("[]");
  test_class<UreStl, UreNfa>("[^]");
  test_class<UreStl, UreNfa>("[^^]");
  test_class<UreStl, UreNfa>("[a-]");
  test_class<UreStl, UreNfa>("[^-a]");
  test_class<UreStl, UreNfa>("[\\^]");
  test_class<UreStl, UreNfa>("[^\\^]");
  test_class<UreStl, UreNfa>("[a\\-b]");
  test_class<UreStl, UreNfa>("[\\-a]");
  test_class<UreStl, UreNfa>("[\\\\]");
  test_class<UreStl, UreNfa>("[\\]]");
  test_class<UreStl, UreNfa>("[.*?|+()]");
  test_class<UreStl, UreNfa>("[aa]");
  test_class<UreStl, UreNfa>("[a-za-z]");

  test_all_regexes<UreStl, UreRecursive>("abc.+*?()|\\", 4, "abcd", 4);
}

TEST(UreTest, TestNfa) {
  UreNfa ure("a(bb)+a");
  ASSERT_FALSE(ure.parsing_failed());
  ASSERT_TRUE(ure.full_match("abbbba"));
  ASSERT_FALSE(ure.full_match("abbba"));
  ASSERT_FALSE(ure.full_match("zzzabbbbazzz"));
  ASSERT_TRUE(ure.partial_match("zzzabbbbazzz"));
  ASSERT_FALSE(ure.partial_match("zzzabbbazzz"));

  UreNfa bad("a(b");
  ASSERT_TRUE(bad.parsing_failed());
  ASSERT_EQ(1, bad.parser_error_info().idx);

  ASSERT_TRUE(UreNfa("abc").partial_match("\nabc\n"));
  test_class<UreStl, UreNfa>(".");
  test_class<UreStl, UreNfa>("\\d");
  test_class<UreStl, UreNfa>("\\D");
  test_class<UreStl, UreNfa>("\\s");
  test_class<UreStl, UreNfa>("\\S");
  test_class<UreStl, UreNfa>("\\w");
  test_class<UreStl, UreNfa>("\\W");

  test_class<UreStl, UreNfa>("[ab]");
  test_class<UreStl, UreNfa>("[^ab]");
  test_class<UreStl, UreNfa>("[a-z]");
  test_class<UreStl, UreNfa>("[a-zA-Z]");
  test_class<UreStl, UreNfa>("[^a A-Z$0-9]");
  test_class<UreStl, UreNfa>("[]");
  test_class<UreStl, UreNfa>("[^]");
  test_class<UreStl, UreNfa>("[^^]");
  test_class<UreStl, UreNfa>("[a-]");
  test_class<UreStl, UreNfa>("[^-a]");
  test_class<UreStl, UreNfa>("[\\^]");
  test_class<UreStl, UreNfa>("[^\\^]");
  test_class<UreStl, UreNfa>("[a\\-b]");
  test_class<UreStl, UreNfa>("[\\-a]");
  test_class<UreStl, UreNfa>("[\\\\]");
  test_class<UreStl, UreNfa>("[\\]]");
  test_class<UreStl, UreNfa>("[.*?|+()]");
  test_class<UreStl, UreNfa>("[aa]");
  test_class<UreStl, UreNfa>("[a-za-z]");

  test_all_regexes<UreStl, UreNfa>("abc.+*?()|\\", 4, "abcd", 4);
}