#include <stdio.h>
#include "board.h"
//#include "generator.h"
//#include "ui.h"

int main(void) {
    Board* b = board_create(9);
    board_set(b, 0, 0, 5);
    board_print(b);
    board_destroy(b);

    return 0;
}