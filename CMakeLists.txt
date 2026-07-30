cmake_minimum_required(VERSION 3.28)
project(Bomberman_AP)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(logic)
add_subdirectory(representation)

enable_testing()
add_subdirectory(tests)
