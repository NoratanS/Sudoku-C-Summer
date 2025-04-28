#ifndef GENERATOR_H
#define GENERATOR_H

#include "board.h"
#include <stdbool.h>

// Creates and fully fills size x size board
// Allocates new Board* and returns NULL if error occurs
Board* generator_full_board(int size);

// Removes k cells from board so it has exactly one solution.
// Return true if it does so successfully
bool generator_remove_cells(Board* full, int k);

#endif //GENERATOR_H
