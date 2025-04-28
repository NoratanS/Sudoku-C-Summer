#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

// Board representation
typedef struct {
  int size;    // for example: 9
  int* cells;  // size*size array
} Board;

// Creates size x size board (malloc)
Board* board_create(int size);

// Frees memory
void board_destroy(Board* board);

// Sets value (1..size) on (row, col)
bool board_set(Board* board, int row, int col, int value);

// Gets value from (row, col) cell (0 if cell is empty)
int board_get(const Board* board, int row, int col);

// Checks if setting (row, col) cell to value is a valid move
bool board_is_valid_move(const Board* board, int row, int col, int value);

// Checks whether all cells are filled
bool board_is_complete(const Board* board);

// Converts label ex. 'A1' to (row, col) format. False if label is not valid
bool board_label_to_indices(const Board* board, const char* label, int* row, int* col);

// Prints the board
void board_print(const Board* board);

#endif //BOARD_H


