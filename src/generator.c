#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "generator.h"
#include "solver.h"

// Forward declaration
static bool fill_cell(Board* b, int row, int col);

// Fisher–Yates arr shuffle
static void shuffle(int* arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
    }
}

Board* generator_full_board(int size) {
    if (size <= 0) return NULL;
    srand((unsigned)time(NULL));

    Board* b = board_create(size);
    if (!b) {
        fprintf(stderr, "generator_full_board: board_create returned NULL");
        return NULL;
    }
    if (!fill_cell(b, 0, 0)) {
        fprintf(stderr, "generator_full_board: fill_cell didnt work");
        board_destroy(b);
        return NULL;
    }
    fprintf(stderr, "generator_full_board: filled the board");
    return b;
}

// Recursive backtracking: Fills cells from (row,col)
static bool fill_cell(Board* b, int row, int col) {
    int n = b->size;
    if (row == n) return true;  // all rows filled

    int next_row = (col == n - 1) ? row + 1 : row;
    int next_col = (col == n - 1) ? 0 : col + 1;

    // prep candidates [1..n]
    int* cands = malloc(n * sizeof(int));
    if (!cands) return false;
    for (int i = 0; i < n; i++) cands[i] = i + 1;
    shuffle(cands, n);

    for (int i = 0; i < n; i++) {
        int v = cands[i];
        if (board_is_valid_move(b, row, col, v)) {
            board_set(b, row, col, v);
            if (fill_cell(b, next_row, next_col)) {
                free(cands);
                return true;
            }
            board_set(b, row, col, 0);
        }
    }
    free(cands);
    return false;
}

// removes k cells with one feasible solution
bool generator_remove_cells(Board* full, int k) {
    if (!full) return false;
    int n = full->size;
    int total = n * n;
    if (k > total) k = total;

    int* indices = malloc(total * sizeof(int));
    if (!indices) return false;
    for (int i = 0; i < total; i++) indices[i] = i;
    shuffle(indices, total);

    int removed = 0;
    for (int i = 0; i < total && removed < k; i++) {
        int idx = indices[i];
        int r = idx / n;
        int c = idx % n;
        int backup = board_get(full, r, c);
        board_set(full, r, c, 0);
        if (count_solutions(full) == 1) {
            removed++;
        } else {
            board_set(full, r, c, backup);
        }
    }
    free(indices);
    return (removed == k);
}
