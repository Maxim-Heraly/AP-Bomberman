cmake_minimum_required(VERSION 3.28)
project(Bomberman CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# TODO: once you've set up clang-format (see .clang-format at the repo root -
# replace it with the exact config linked in the assignment PDF, section
# 3.2), consider adding a `format` custom target here that runs it
# automatically over logic/ and representation/.

add_subdirectory(logic)
add_subdirectory(representation)

enable_testing()
add_subdirectory(tests)
