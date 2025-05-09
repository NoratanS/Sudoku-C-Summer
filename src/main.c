#include <stdio.h>
#include "board.h"
#include "generator.h"
#include "solver.h"
#include "ui.h"
#include<stdlib.h>
#include<math.h>

typedef struct {
    Board* board;
    int cost;
} SudokuState;

bool is_valid_num(Board* board, int row, int col, int value) {
    if (!board || value < 1 || value > board->size || row < 0 || row >= board->size || col < 0 || col >= board->size) {
        return false;
    }

    int n = board->size;

    // Sprawdzanie wiersza (pomijamy kolumnę 'col')
    for (int i = 0; i < n; i++) {
        if (i != col && board_get(board, row, i) == value) {
            return false; // Konflikt w wierszu
        }
    }

    // Sprawdzanie kolumny (pomijamy wierszę 'row')
    for (int i = 0; i < n; i++) {
        if (i != row && board_get(board, i, col) == value) {
            return false; // Konflikt w kolumnie
        }
    }

    // Sprawdzanie regionu 3x3
    int section = (int) sqrt(n);
    int startR = (row / section) * section;
    int startC = (col / section) * section;
    for (int dr = 0; dr < section; dr++) {
        for (int dc = 0; dc < section; dc++) {
            int r = startR + dr;
            int c = startC + dc;
            if (r != row || c != col) {
                if (board_get(board, r, c) == value) {
                    return false; // Konflikt w regionie 3x3
                }
            }
        }
    }

    return true; // Jeśli nie ma konfliktu, zwróć true
}
void calculate_cost(SudokuState *sudokuState) {
    if (!sudokuState->board) fprintf(stderr, "BRRRR");
    int n = sudokuState->board->size;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int current_value = board_get(sudokuState->board, i, j);
            if (is_valid_num(sudokuState->board, i, j, current_value) == false) {
                sudokuState->cost += 1;
            }
        }
    }
}



int main(void) {

    Board* board_test = generator_full_board(9);
    if (board_test == NULL) {
        fprintf(stderr, "Nie udało się utworzyć planszy.\n");
        return 1;
    }

    generator_remove_cells(board_test, 20);
    board_print(board_test);

    SudokuState *stest = malloc(sizeof(SudokuState));
    if (!stest) {
        fprintf(stderr, "Nie udało się alokować pamięci dla stanu Sudoku.\n");
        return 1;
    }

    stest->board = board_test;
    stest->cost = 0;

    calculate_cost(stest);
    int cost = stest->cost;
    printf("Koszt: %d\n", cost);

    free(stest); // Pamiętaj o zwolnieniu pamięci

    //ui_start();
    return 0;
}