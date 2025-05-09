//
// Created by kbiel on 09/05/2025.
//

#ifndef SOLVER_SA_H
#define SOLVER_SA_H
#include "board.h"
#include <stdbool.h>

bool solver_sa(Board* board,
               double t_start,
               double t_end,
               double alpha,
               int max_iters);
#endif //SOLVER_SA_H
