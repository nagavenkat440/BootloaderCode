cmake_minimum_required(VERSION 3.16)

project(BootloaderTests)

add_subdirectory(googletest)

enable_testing()

add_executable(runTests
    Tests/test_post.cpp
    Core/Src/crc_utils.c
)

target_include_directories(runTests PRIVATE
    Core/Inc
)

target_include_directories(runTests PRIVATE
    Tests/mocks
    Core/Inc
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