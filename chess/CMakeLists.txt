# CMakeList.txt : CMake project for space, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_library (chess "board.h" "board_impl.h" "board_impl.cpp" "fen.h" "fen.cpp" 
					"algo.h" "algo.cpp" "algoLinear.h" "algoLinear.cpp")
# target_link_libraries (chess common)
target_include_directories (chess PUBLIC ..)

# TODO: Add tests and install targets if needed.



