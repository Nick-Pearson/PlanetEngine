
file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_executable(planet-test ${TEST_SOURCES})

target_link_libraries(planet-test ${CONAN_LIBS})

add_test(NAME planet-test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMAND planet-test)