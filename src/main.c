#include <stdio.h>
#include "board.h"
#include "generator.h"
#include "generator.c"
#include "solver.h"
#include "ui.h"
#include<stdlib.h>
#include<math.h>
#include <string.h>
#include "solver_sa.c"



int main(void) {
    int n = 9;
    Board* board_test = generator_full_board(n);
    if (board_test == NULL) {
        fprintf(stderr, "board generation failed.\n");
        return 1;
    }

    generator_remove_cells(board_test, 64);
    board_print(board_test);

    bool found_sol = solver_sa(board_test, 3, 0.01, 0.999, 20000000);
    if (found_sol) {
        printf("\nSolution found\n");
        printf("Cost: %d\n", calculate_cost(board_test));
        board_print(board_test);
    }
    else {
        printf("\nFailed, best found:\n");
        printf("Cost: %d\n", calculate_cost(board_test));
        board_print(board_test);
    }

    //ui_start();
    return 0;
}