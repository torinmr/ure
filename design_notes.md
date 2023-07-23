# Design notes

## Character classes

I want to add support for character classes, which includes the following functionality:

    User defined character classes, such as:
    ```
      [abc]     Matches a, b, or c
      [a-z]     Matches all lowercase letters
      [a-z0-9X] Matches lowercase letter, digits, and capital X
      [^abc]    Matches any character except a, b, or c.
    ```

    Built-in character classes, such as:
    ```
    \d  Matches decimal digits
    \s  Matches space characters
    ```

In both cases, the entire character class matches only a single character of the text.

A note on semantics: Real regex implementations have some slightly complicated rules 
for escape characters in character classes. ], \, -, and ^ are the only special 
characters, and can be escaped, but implementations will allow them to be used unescaped
if it can be figured out from context - for example:

* ^ can be used without escaping if it's not the first character, and in some 
  implementations can be used as the first character if it's the only character.
* '-' can be used unescaped if it's not in a location where a - would be.

I'd propose the following rules for my version:

* Reserved characters not used in character classes (.|? etc.) can optionally be 
  escaped inside character classes, but it's not necessary.
* [] is valid (matches no characters), as is [^] (matches any character).
* ^ must be escaped if it's the first character, otherwise it can be unescaped.
* ] must always be escaped. []a] will be parsed as [] followed by some junk that
  fails to parse.
* '-' can be placed unescaped at beginning or end, or right after ^. In any other 
  position where it can't form a valid ranges it's an error.
* \ must always be escaped.

This gives the following EBNF grammar:

Class                = "[", (NegatedClass | Class) "]"
NegatedClass         = "^", ClassContents
ClassContents        = ["-"], [ClassElements], ["-"]
ClassElements        = ClassElement, [ClassElements]
ClassElement         = ClassRange | ClassChar
ClassRange           = ClassChar, "-", ClassChar
ClassChar            = ClassEscape | ClassLiteral
ClassEscape          = "\", NonAlphaNum
ClassLiteral         = "a" | "b" | ... (not "]", "\", "-")

Also, we need to add [, ] to ReservedLiteral.

### Implementation options

Character classes could be implemented with alternation - e.g. [abc] becomes
(a|b|c). But this would cause very large expansions (e.g. [a-z] would expand to 26
alternations), and I'm not sure how we would handle negations, so we'll need to add
some special extensions to the instruction set.

The other extreme would be to have a single instruction for the entire character class,
but the instruction would have to be quite complex. Here's what a parsed representaion
might look like:

```c++
struct CharClass {
  bool negated;
  string characters;
  vector<pair<char, char>> ranges;
};
```

Actually, that's not that bad, and I guess adding a single pointer to a CharClass 
wouldn't be awful. One concern would be if we added lots more instructions like this in
the future, we'd add lots of pointers to every instruction. Maybe there could be a
pointer to an empty base type that CharClass and other similar structures in the future
all pointed to, like this:

```c++
struct Instruction {
  ...
  unique_ptr<ExtraInfo> extra;
  
  bool match(char c) {
    assert(type == IType::CharClass);
    static_cast<*CharClass>(extra.get())->match(c);
  }
};

struct ExtraInfo {};

struct CharClass : ExtraInfo {
  ...
}
```

Needs a bit of work, obviously. But it's definitely not necessary right now.

Another alternative would be to have a simpler CharClass struct that only 
corresponds to one part of the class specifier, and compose a list of them. I think 
this would be inefficient and not simplify things a bunch.

As a final alternative, consider if the CharClass instruction had a string with the 
whole unparsed character class, e.g. "abc" in the first example, and the 
implementation of the instruction parsed it on the fly. Code might look something 
like this:

```c++
bool match(const string& s, char c) {
  size_t idx = 0;
  bool negate = false;
  if (s[0] == '^') {
    idx++;
    negate = true;
  }
  
  while (idx < s.size()) {
    if (idx == size() - 2 || s[idx+1] != '-') {
      if (s[idx] == c) return !negate;
    } else {
      if (s[idx] <= c && c <= s[idx+2]) return !negate;
    }
  }
}
```

This honestly isn't too bad, but once you factor in handling escape sequences, checking
for invalid character classes, etc., it's probably something you want handled in a 
separate parse phase.

For built-in character classes (like \s etc.), I think I could extend the Wildcard
instruction to allow setting the char for the intruction. So Wildcard('s') would 
match space characters, Wildcard('.') would be the generic wildcard, etc.
