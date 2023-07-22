cc_library(
  name = "instruction",
  hdrs = ["instruction.h"],
  srcs = ["instruction.cc"],
)

cc_test(
  name = "instruction_test",
  size = "small",
  srcs = ["instruction_test.cc"],
  deps = [
    "@com_google_googletest//:gtest_main",
    ":instruction",
  ],
)

cc_library(
  name = "parser",
  hdrs = ["parser.h"],
  srcs = ["parser.cc"],
  deps = [":instruction"],
)

cc_test(
  name = "parser_test",
  size = "small",
  srcs = ["parser_test.cc"],
  deps = [
    "@com_google_googletest//:gtest_main",
    ":parser",
  ],
)

cc_library(
  name = "ure_interface",
  hdrs = ["ure_interface.h"],
)

cc_library(
  name = "ure_stl",
  hdrs = ["ure_stl.h"],
  deps = [":ure_interface"],
)

cc_library(
  name = "ure_recursive",
  hdrs = ["ure_recursive.h"],
  srcs = ["ure_recursive.cc"],
  deps = [
    ":parser",
    ":ure_interface",
  ],
)

cc_test(
  name = "ure_test",
  size = "medium",
  srcs = ["ure_test.cc"],
  deps = [
    "@com_google_googletest//:gtest_main",
    ":ure_recursive",
    ":ure_stl",
  ],
)