#include "solver.h"
#include <stdlib.h>

// Recursive
static bool solve_recursive(Board* board, int row, int col) {
    int n = board->size;
    // If steps over last row - solution is found
    if (row == n) return true;

    int next_row = (col == n - 1) ? row + 1 : row;
    int next_col = (col == n - 1) ? 0 : col + 1;

    // If cell already has a value go to the next one
    if (board_get(board, row, col) != 0) {
        return solve_recursive(board, next_row, next_col);
    }

    // Try each value 1..n
    for (int v = 1; v <= n; v++) {
        if (board_is_valid_move(board, row, col, v)) {
            board_set(board, row, col, v);
            if (solve_recursive(board, next_row, next_col)) return true;
            board_set(board, row, col, 0);
        }
    }
    // No feasalbe value => backtrack
    return false;
}

bool solver_solve(Board* board) {
    if (!board) return false;
    return solve_recursive(board, 0, 0);
}

// Recursive solution count, stops after limit
static int count_recursive(Board* board, int row, int col, int limit) {
    int n = board->size;
    if (row == n) return 1;

    int next_row = (col == n - 1) ? row + 1 : row;
    int next_col = (col == n - 1) ? 0 : col + 1;

    if (board_get(board, row, col) != 0) {
        return count_recursive(board, next_row, next_col, limit);
    }

    int count = 0;
    for (int v = 1; v <= n; v++) {
        if (board_is_valid_move(board, row, col, v)) {
            board_set(board, row, col, v);
            count += count_recursive(board, next_row, next_col, limit - count);
            // If limit is crossed, stop
            if (count >= limit) {
                board_set(board, row, col, 0);
                return count;
            }
            board_set(board, row, col, 0);
        }
    }
    return count;
}

int count_solutions(Board* board) {
    if (!board) return 0;
    return count_recursive(board, 0, 0, 2);
}