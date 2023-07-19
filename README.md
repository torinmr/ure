# URE: The Unoriginal Regex Engine

A project with various implementations of a regular expression matching interface, based on this
[series of articles](https://swtch.com/~rsc/regexp/) by Russ Cox (the original author of
Google's [RE2](https://github.com/google/re2/) library).

This is a learning project to better understand regex engine internals, I don't suggest using it for
anything serious (RE2 or the built-in STL regex library are better choices).

# Building and testing

Prerequisites: Install [Bazel](https://bazel.build/install)

```shell
$ ./test.sh
```