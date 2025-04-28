add_library(sudoku
        board.c
        generator.c
        solver.c
        ui.c
)

target_include_directories(sudoku PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(run_sudoku main.c)
target_link_libraries(run_sudoku PRIVATE sudoku)