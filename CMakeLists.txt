cmake_minimum_required(VERSION 2.8.12)
project(PlanetEngine)

# Initialize Conan.io.
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_compile_definitions(PLATFORM_WIN=1)

file(GLOB_RECURSE ENG_SOURCES "Src/Planet-Engine/Private/*.cpp")
add_library(planet-engine STATIC ${ENG_SOURCES})
target_link_libraries(planet-engine ${CONAN_LIBS})
target_include_directories(planet-engine PRIVATE "Src/Planet-Engine/Private")
include_directories("Src/Planet-Engine/Public")

file(GLOB_RECURSE D3D_SOURCES "Src/D3DRenderer/Private/*.cpp")
add_library(d3d-renderer STATIC ${D3D_SOURCES})
target_include_directories(d3d-renderer PRIVATE "Src/D3DRenderer/Private")
include_directories("Src/D3DRenderer/Public")
target_link_libraries(d3d-renderer ${CONAN_LIBS})
target_link_libraries(d3d-renderer planet-engine)

# # Setup testing.
file(GLOB_RECURSE TEST_SOURCES "Src/Planet-Engine/Test/*.cpp")
add_executable(planet-test ${TEST_SOURCES})
target_include_directories(planet-test PRIVATE "Src/Planet-Engine/Private")
target_link_libraries(planet-test ${CONAN_LIBS})
target_link_libraries(planet-test planet-engine)
enable_testing()
add_test(NAME planet-test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMAND planet-test)

add_executable(planet-game WIN32 "Src/Planet-Game/Main.cpp")
target_link_libraries(planet-game d3d-renderer)
target_link_libraries(planet-game planet-engine)
target_link_libraries(planet-game shcore.lib)
