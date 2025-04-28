cmake_minimum_required(VERSION 3.15)
project(sudoku-c LANGUAGES C)

# set C standard
if(NOT CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED ON)
endif()

# enable warnings
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

# source directories
add_subdirectory(src)
add_subdirectory(tests)