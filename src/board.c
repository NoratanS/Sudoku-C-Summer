#include "board.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include<math.h>

Board* board_create(int size) {
    if (size <= 0) return NULL;
    Board* board = malloc(sizeof(Board));
    if (!board) return NULL;
    board->size = size;
    board->cells = calloc(size * size, sizeof(int));
    if (!board->cells) {
        free(board);
        return NULL;
    }
    return board;
}

void board_destroy (Board* board) {
    if (!board) return;
    free(board->cells);
    free(board);
}

bool board_set(Board* board, int row, int col, int value) {
    if (!board || row < 0 || col < 0 || row >= board->size || col >= board->size) return false;
    board->cells[row * board->size + col] = value;
    return true;
}

int board_get(const Board* board, int row, int col) {
    if (!board || row < 0 || col < 0 || row >= board->size) return 0;
    return board->cells[row * board->size + col];
}

bool board_is_valid_move(const Board* board, int row, int col, int value) {
    if (!board || value < 1 || value > board->size) return false;
    int n = board->size;

    // row and col
    for (int i = 0; i < n; i++) {
        if (board_get(board, row, i) == value) return false;
        if (board_get(board, i, col) == value) return false;
    }

    // section
    int section = (int) sqrt(n);
    int startR = (row / section) * section;
    int startC = (col / section) * section;
    for (int dr=0; dr < section; dr++) {
        for (int dc=0; dc < section; dc++) {
            if (board_get(board, startR + dr, startC + dc) == value) return false;
        }
    }
    return true;
}

bool board_is_complete(const Board* board) {
    if (!board) return false;
    int total = board->size * board->size;
    for (int i = 0; i < total; i++) {
        if (board->cells[i] == 0) return false;
    }
    return true;
}

bool board_label_to_indicies(const Board* board, const char* label, int* row, int* col) {
    if (!board || !label || strlen(label) < 2) return false;
    char letter = toupper(label[0]);
    int c = letter - 'A';
    int r = atoi(label+1) -1;
    if (c<0 || c>= board->size || r<0 || r>= board->size) return false;
    *row = r;
    *col = c;
    return true;
}

void board_print(const Board* board) {
    if (!board) return;
    int n = board->size;

    // Col label
    printf("   ");
    for (int c=0; c < n; c++) printf(" %c", 'A'+c);
    printf("\n");
    for (int r=0; r<n; r++) {
        printf("%2d ", r+1);
        for (int c=0; c<n; c++) {
            int v = board_get(board, r, c);
            if (v>0) printf(" %d", v);
            else printf(" .");
        }
        printf("\n");
    }
}