#ifndef SOLVER_H
#define SOLVER_H

#include "board.h"
#include <stdbool.h>

// Solves sudoku board using backtracking
// Returns true if solution is found (modifies the board)
bool solver_solve(Board* b);

// Counts feasible solutions, stops when reaches 2.
// used for generator.
int count_solutions(Board* b);

#endif //SOLVER_H
