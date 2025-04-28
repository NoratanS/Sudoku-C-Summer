cmake_minimum_required(VERSION 3.15)
project(Sudoku-C-Summer LANGUAGES C)

# set C standard
if(NOT CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED ON)
endif()

include_directories(include)

# source directories
add_subdirectory(src)
#add_subdirectory(tests)