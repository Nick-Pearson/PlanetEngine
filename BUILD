load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

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
    visibility = ["//visibility:public"],
)

cc_library(
    name = "d3d-renderer",
    deps = [":planet-engine", "//Lib:imgui", "//Lib:stb"],
    srcs = glob([
        "Src/D3DRenderer/Private/**/*.cpp",
    ]),
    includes = [
        "Src/D3DRenderer/Private/",
        "Src/D3DRenderer/Public/",
    ],
    hdrs = glob([
        "Src/D3DRenderer/Public/**/*.h",
    ]),
    defines = [],
    visibility = ["//visibility:private"],
)

cc_binary(
    name = "planet-game",
    deps = [":planet-engine", ":d3d-renderer"],
    srcs = [
        "Src/Planet-Game/Main.cpp",
    ],
    linkopts = ["-SUBSYSTEM:WINDOWS"]
)