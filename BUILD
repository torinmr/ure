cc_library(
  name = "ure_interface",
  hdrs = ["ure_interface.h"],
)

cc_library(
  name = "ure_stl",
  hdrs = ["ure_stl.h"],
  deps = [":ure_interface"],
)

cc_test(
  name = "ure_test",
  size = "small",
  srcs = ["ure_test.cc"],
  deps = [
    "@com_google_googletest//:gtest_main",
    ":ure_stl",
  ],
)