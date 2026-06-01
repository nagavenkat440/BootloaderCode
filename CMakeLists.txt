cmake_minimum_required(VERSION 3.16)

project(BootloaderTests)

add_subdirectory(googletest)

enable_testing()

add_executable(runTests
Tests/test_crc.cpp
)

target_compile_options(runTests PRIVATE
    --coverage
)

target_link_options(runTests PRIVATE
    --coverage
)

target_link_libraries(runTests
gtest
gtest_main
)

include(GoogleTest)

gtest_discover_tests(runTests)


