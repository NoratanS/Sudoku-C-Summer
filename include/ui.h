#ifndef UI_H
#define UI_H

#include "board.h"
#include <time.h>

// Stores Game State
typedef struct {
    Board* board;
    int    size;        // board size
    int    hints;       // available hints
    int    difficulty;  // 1=Easy,2=Medium,3=Hard
    int    moves;
    time_t start_time;
} GameState;


enum { UI_QUIT = 0, UI_CONTINUE };
void ui_start(void);

#endif // UI_H