cmake_minimum_required (VERSION 3.8)

add_executable (game_cli "game_cli.cpp" "CliAlgo.h" "CliAlgo.cpp")
target_link_libraries (game_cli chess algo_linear)
