#include <memory>

#include <gtest/gtest.h>

#include "parser.h"

using namespace std;
using namespace ure;

TEST(ParserTest, RegexCompare) {
  ASSERT_EQ(Regex(NType::Wildcard), Regex(NType::Wildcard));
  ASSERT_EQ(Regex(NType::Literal, 'a'), Regex(NType::Literal, 'a'));
  ASSERT_EQ(Regex(NType::Star, make_unique<Regex>(NType::Wildcard)),
            Regex(NType::Star, make_unique<Regex>(NType::Wildcard)));
  ASSERT_EQ(Regex(NType::Alternate,
                  make_unique<Regex>(NType::Wildcard),
                  make_unique<Regex>(NType::Literal, 'a')),
            Regex(NType::Alternate,
                  make_unique<Regex>(NType::Wildcard),
                  make_unique<Regex>(NType::Literal, 'a')));

  ASSERT_NE(Regex(NType::Literal, 'a'), Regex(NType::Literal, 'b'));
  ASSERT_NE(Regex(NType::Literal, 'a'), Regex(NType::Wildcard));
  ASSERT_NE(Regex(NType::Star, make_unique<Regex>(NType::Literal, 'a')),
            Regex(NType::Star, make_unique<Regex>(NType::Literal, 'b')));
  ASSERT_NE(Regex(NType::Alternate,
                  make_unique<Regex>(NType::Wildcard),
                  make_unique<Regex>(NType::Literal, 'a')),
            Regex(NType::Question, make_unique<Regex>(NType::Wildcard)));
  ASSERT_NE(Regex(NType::Alternate,
                  make_unique<Regex>(NType::Wildcard),
                  make_unique<Regex>(NType::Literal, 'a')),
            Regex(NType::Alternate,
                  make_unique<Regex>(NType::Wildcard),
                  make_unique<Regex>(NType::Literal, 'b')));
  ASSERT_NE(Regex(NType::Alternate,
                  make_unique<Regex>(NType::Literal, 'a'),
                  make_unique<Regex>(NType::Wildcard)),
            Regex(NType::Alternate,
                  make_unique<Regex>(NType::Literal, 'b'),
                  make_unique<Regex>(NType::Wildcard)));
}

TEST(ParserTest, ValidParse) {
  Parser parser;
  unique_ptr<Regex> re = parser.parse("a(bb*)+a|.?");
  ASSERT_NE(re, nullptr);
  Regex expected(
    NType::Alternate,
    make_unique<Regex>(
      NType::Concat,
      make_unique<Regex>(NType::Literal, 'a'),
      make_unique<Regex>(
        NType::Concat,
        make_unique<Regex>(
          NType::Plus,
          make_unique<Regex>(
            NType::Alternate,
            make_unique<Regex>(
              NType::Concat,
              make_unique<Regex>(NType::Literal, 'b'),
              make_unique<Regex>(
                NType::Concat,
                make_unique<Regex>(
                  NType::Star,
                  make_unique<Regex>(NType::Literal, 'b')
                )
              )
            )
          )
        ),
        make_unique<Regex>(
          NType::Concat,
          make_unique<Regex>(NType::Literal, 'a')
        )
      )
    ),
    make_unique<Regex>(
      NType::Alternate,
      make_unique<Regex>(
        NType::Concat,
        make_unique<Regex>(
          NType::Question,
          make_unique<Regex>(NType::Wildcard)
        )
      )
    )
  );
  ASSERT_EQ(*re, expected);
}

TEST(ParserTest, EmptyParse) {
  Parser parser;

  ASSERT_EQ(
    *parser.parse(""),
    Regex(
      NType::Alternate,
      make_unique<Regex>(NType::Empty)
    )
  );

  ASSERT_EQ(
    *parser.parse("|"),
    Regex(
      NType::Alternate,
      make_unique<Regex>(NType::Empty),
      make_unique<Regex>(
        NType::Alternate,
        make_unique<Regex>(NType::Empty)
      )
    )
  );

  ASSERT_EQ(
    *parser.parse("()"),
    Regex(
      NType::Alternate,
      make_unique<Regex>(
        NType::Concat,
        make_unique<Regex>(
          NType::Alternate,
          make_unique<Regex>(NType::Empty)
        )
      )
    )
  );

  ASSERT_EQ(
    *parser.parse("()*"),
    Regex(
      NType::Alternate,
      make_unique<Regex>(
        NType::Concat,
        make_unique<Regex>(
          NType::Star,
          make_unique<Regex>(
            NType::Alternate,
            make_unique<Regex>(NType::Empty)
          )
        )
      )
    )
  );
}

TEST(ParserTest, InvalidParse) {
  Parser parser;
  ASSERT_EQ(parser.parse("abc??"), nullptr);
  ASSERT_EQ(parser.error_info().idx, 4);
  ASSERT_EQ(parser.error_info().pattern, "abc??");

  ASSERT_EQ(parser.parse("abc("), nullptr);
  ASSERT_EQ(parser.error_info().idx, 3);

  ASSERT_EQ(parser.parse("("), nullptr);
  ASSERT_EQ(parser.error_info().idx, 0);

  ASSERT_EQ(parser.parse("*abc"), nullptr);
  ASSERT_EQ(parser.error_info().idx, 0);

  ASSERT_EQ(parser.parse("(a)b)"), nullptr);
  ASSERT_EQ(parser.error_info().idx, 4);
}