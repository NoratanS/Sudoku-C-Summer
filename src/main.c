#include <stdio.h>
#include "board.h"
#include "generator.h"
#include "solver.h"
//#include "ui.h"

int main(void) {
    Board* full = generator_full_board(9);
    if (!full) {
        fprintf(stderr, "Error: nie udało się wygenerować pełnej planszy\n");
        return 1;
    }

    board_print(full);

    if (!generator_remove_cells(full, 40)) {
        fprintf(stderr, "Error: nie udało się usunąć komórek (full == %p)\n", (void*)full);
        board_destroy(full);
        return 1;
    }

    board_print(full);
    board_destroy(full);

    return 0;
}