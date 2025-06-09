//
// Created by kbiel on 09/06/2025.
//

#ifndef SOLVER_GA_H
#define SOLVER_GA_H
#include "board.h"
#include <stdbool.h>

bool solver_ga(Board *board,
               int    population_size,
               int    generations,
               double crossover_rate,
               double mutation_rate);

#endif //SOLVER_GA_H
