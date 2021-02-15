cmake_minimum_required (VERSION 3.8)

add_executable (chess_test "chess_test.cpp")
target_link_libraries (chess_test chess gtest_main)
add_test (NAME chess_test COMMAND chess_test)
