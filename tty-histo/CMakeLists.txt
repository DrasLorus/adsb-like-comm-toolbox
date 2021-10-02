cmake_minimum_required (VERSION 3.16)

project (
  tty_histo
  VERSION 0.1
  LANGUAGES CXX
)

set (CMAKE_CXX_STANDARD 14)

set (CMAKE_CXX_FLAGS "-Ofast -g0 -march=native -mtune=native")
set (CMAKE_EXE_LINKER_FLAGS "-lncurses")

set (SOURCE_FILES ../src/tty_histo.cpp)

add_executable (tty_histo ${SOURCE_FILES})
target_link_libraries (tty_histo)
