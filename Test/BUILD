load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "planet-test",
    srcs = glob([
            "**/*.cpp"
    ]),
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
    ],
)