cmake_minimum_required (VERSION 3.8)

add_executable (game_cli "game_cli.cpp")
target_link_libraries (game_cli chess)

