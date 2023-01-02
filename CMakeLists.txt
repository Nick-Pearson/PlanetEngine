cmake_minimum_required(VERSION 2.8.12)
project(PlanetEngine)

# Initialize Conan.io.
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_compile_definitions(PLATFORM_WIN=1)

file(GLOB_RECURSE ENG_SOURCES "Src/Planet-Engine/Private/*.cpp")
add_library(planet-engine STATIC ${ENG_SOURCES})
target_compile_features(planet-engine PRIVATE cxx_std_17)
target_link_libraries(planet-engine ${CONAN_LIBS})
target_include_directories(planet-engine PRIVATE "Src/Planet-Engine/Private")
include_directories("Src/Planet-Engine/Public")

file(GLOB_RECURSE D3D_SOURCES "Src/D3DRenderer/Private/*.cpp")
add_library(d3d-renderer STATIC ${D3D_SOURCES})
target_compile_features(d3d-renderer PRIVATE cxx_std_17)
target_include_directories(d3d-renderer PRIVATE "Src/D3DRenderer/Private")
target_include_directories(d3d-renderer PRIVATE "Src/D3DRenderer/Lib")
include_directories("Src/D3DRenderer/Public")
target_link_libraries(d3d-renderer ${CONAN_LIBS})
target_link_libraries(d3d-renderer planet-engine)
target_link_libraries(d3d-renderer D3DCompiler.lib d3d12.lib dxgi.lib dxguid.lib)

# # Setup testing.
file(GLOB_RECURSE TEST_SOURCES "Src/Planet-Engine/Test/*.cpp")
add_executable(planet-test ${TEST_SOURCES})
target_compile_features(planet-test PRIVATE cxx_std_17)
target_include_directories(planet-test PRIVATE "Src/Planet-Engine/Private")
target_include_directories(planet-test PRIVATE "Src/Planet-Engine/Test")
target_link_libraries(planet-test ${CONAN_LIBS})
target_link_libraries(planet-test planet-engine)
enable_testing()
add_test(NAME planet-test
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND planet-test)

file(GLOB_RECURSE GAME_SOURCES "Src/Planet-Game/Private/*.cpp")
add_executable(planet-game WIN32 ${GAME_SOURCES})
target_compile_features(planet-game PRIVATE cxx_std_17)
target_link_libraries(planet-game d3d-renderer)
target_link_libraries(planet-game planet-engine)
target_link_libraries(planet-game shcore.lib)

file(GLOB_RECURSE EDITOR_SOURCES "Src/Planet-Editor/Private/*.cpp")
add_executable(planet-editor WIN32 ${EDITOR_SOURCES})
target_compile_features(planet-editor PRIVATE cxx_std_17)
target_link_libraries(planet-editor ${CONAN_LIBS})
target_link_libraries(planet-editor d3d-renderer)
target_link_libraries(planet-editor planet-engine)
target_link_libraries(planet-editor shcore.lib)