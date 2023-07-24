# URE: The Unoriginal Regex Engine

A project with various implementations of a regular expression matching interface, based on this
[series of articles](https://swtch.com/~rsc/regexp/) by Russ Cox (the original author of
Google's [RE2](https://github.com/google/re2/) library).

This is a learning project to better understand regex engine internals, I don't suggest using it for
anything serious (RE2 or the built-in STL regex library are better choices).

## Current status
I've implemented two different regex engines, one that uses recursive descent (ure_recursive.h), and
another that uses a non-deterministic finite automaton (ure_nfa.h). Both are based on compiling the
source regex into a bytecode representation that can be executed by a simple virtual machine
(instruction.h/parser.h).

Both implementations support the same subset of regular expression features, described in parser.h.

## Building and testing

Prerequisites: Install [Bazel](https://bazel.build/install)

```shell
$ ./test.sh
```
