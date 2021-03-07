# CMakeList.txt : CMake project for common utilities
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_library (common "base.h" "base.cpp")
target_include_directories (common PUBLIC ..)

# TODO: Add tests and install targets if needed.
