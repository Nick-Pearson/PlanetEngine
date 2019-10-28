load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test", "cc_binary")

cc_library(
    name = "planet-engine",
    deps = ["//Lib:imgui", "//Lib:stb"],
    srcs = glob([
        "Src/Planet-Engine/Private/**/*.cpp",
    ]),
    includes = [
        "Src/Planet-Engine/Private/",
        "Src/Planet-Engine/Public/",
    ],
    hdrs = glob([
        "Src/Planet-Engine/Public/**/*.h",
    ]),
    defines = ["PLATFORM_WIN=1"],
)

cc_binary(
    name = "planet-game",
    deps = [":planet-engine"],
    srcs = [
        "Src/Planet-Game/WinMain.cpp",
    ],
    linkopts = ["-SUBSYSTEM:WINDOWS"]
)