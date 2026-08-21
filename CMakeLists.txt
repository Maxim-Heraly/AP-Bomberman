cmake_minimum_required(VERSION 3.28)
project(Bomberman CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(logic)
add_subdirectory(representation)

enable_testing()
add_subdirectory(tests)
