# CMakeList.txt : CMake project for space, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_library (chess "board.h" "board_impl.h" "board_impl.cpp")

# TODO: Add tests and install targets if needed.
