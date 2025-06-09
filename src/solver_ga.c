#include "solver_ga.h"
#include "board.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* ----------  borrowed from solver_sa.c ------ */
#define VERBOSE_GA 1
/* Fisher‑Yates shuffle for an int array */
static void shuffle_int_ga(int *arr, int n) {
    for (int i = n - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
    }
}

// Calculates the cost (number of row/column conflicts) of the board
static int calculate_cost_ga(const Board *b) {
    int n = b->size;
    int cost = 0;
    int count[17];               // n ≤ 16

    /* rows */
    for (int row = 0; row < n; ++row) {
        memset(count, 0, sizeof(count));
        for (int col = 0; col < n; ++col) {
            int val = board_get(b, row, col);
            if (val) ++count[val];
        }
        for (int val = 1; val <= n; ++val) if (count[val] > 1) cost += count[val] - 1;
    }
    /* cols */
    for (int col = 0; col < n; ++col) {
        memset(count, 0, sizeof(count));
        for (int row = 0; row < n; ++row) {
            int val = board_get(b, row, col);
            if (val) ++count[val];
        }
        for (int val = 1; val <= n; ++val) if (count[val] > 1) cost += count[val] - 1;
    }
    return cost;
}

/* Fill every block with a random permutation of the missing digits */
static void random_fill_blocks_ga(Board *b) {
    int n = b->size;
    int block = (int)sqrt(n);

    int missing[16], miss_count;
    int empties[16], empty_count;

    for (int br = 0; br < block; ++br) {
        for (int bc = 0; bc < block; ++bc) {
            bool present[17] = {0};
            miss_count = empty_count = 0;
            /* scan block */
            for (int dr = 0; dr < block; ++dr) {
                for (int dc = 0; dc < block; ++dc) {
                    int row = br * block + dr;
                    int col = bc * block + dc;
                    int val = board_get(b, row, col);
                    if (val) present[val] = true; else empties[empty_count++] = row * n + col;
                }
            }
            for (int val = 1; val <= n; ++val) if (!present[val]) missing[miss_count++] = val;
            shuffle_int_ga(missing, miss_count);
            for (int i = 0; i < miss_count; ++i) {
                int idx = empties[i];
                board_set(b, idx / n, idx % n, missing[i]);
            }
        }
    }
}

/* swap two mutable cells inside one random block; returns true on success */
static bool swap_in_block_ga(Board *b, const bool *fixed) {
    int n = b->size;
    int block = (int)sqrt(n);

    for (int attempt = 0; attempt < block + 1; ++attempt) {
        int br = rand() % block, bc = rand() % block;
        int cand[16], cnt = 0;
        for (int dr = 0; dr < block; ++dr)
            for (int dc = 0; dc < block; ++dc) {
                int r = br * block + dr, c = bc * block + dc, idx = r * n + c;
                if (!fixed[idx]) cand[cnt++] = idx;
            }
        if (cnt < 2) continue;
        int i = rand() % cnt, j;
        do { j = rand() % cnt; } while (j == i);
        int idx1 = cand[i], idx2 = cand[j];
        int r1 = idx1 / n, c1 = idx1 % n;
        int r2 = idx2 / n, c2 = idx2 % n;
        int v1 = board_get(b, r1, c1);
        int v2 = board_get(b, r2, c2);
        board_set(b, r1, c1, v2);
        board_set(b, r2, c2, v1);
        return true;
    }
    return false;
}

/* -------------------------- GA implementation -------------------------- */

typedef struct {
    Board *board;
    int    cost;
} Individual;

/* Evaluate (or re‑evaluate) an individual */
static void eval_individual(Individual *ind) {
    ind->cost = calculate_cost_ga(ind->board);
}

/* Tournament selection – returns index of winner */
static int tournament_select_ga(Individual *pop, int pop_size, int k) {
    int best = rand() % pop_size;
    for (int i = 1; i < k; ++i) {
        int c = rand() % pop_size;
        if (pop[c].cost < pop[best].cost) best = c;
    }
    return best;
}

/* Uniform crossover – child inherits each mutable gene randomly */
static void crossover_uniform_ga(const Board *pA, const Board *pB, Board *child,
                                 const bool *fixed) {
    int n = pA->size;
    /* START : child == parent A */
    memcpy(child->cells, pA->cells, sizeof(int)*n*n);
    for (int idx = 0; idx < n*n; ++idx) {
        if (fixed[idx]) continue;
        if (rand() & 1) child->cells[idx] = pB->cells[idx];
    }
}

bool solver_ga(Board *puzzle,
               int    population_size,
               int    generations,
               double crossover_rate,
               double mutation_rate) {
    if (!puzzle || population_size < 2) return false;

    srand((unsigned)time(NULL));

    int n = puzzle->size;
    int total = n*n;

    /* fixed cells mask */
    bool *fixed = calloc(total, sizeof(bool));
    for (int i = 0; i < total; ++i) fixed[i] = (puzzle->cells[i] != 0);

    /* Allocate population */
    Individual *pop  = calloc(population_size, sizeof(Individual));
    Individual *next = calloc(population_size, sizeof(Individual));

    /* --- initial population ------------------------------------------------ */
    for (int i = 0; i < population_size; ++i) {
        Board *b = board_create(n);
        memcpy(b->cells, puzzle->cells, sizeof(int)*total);
        random_fill_blocks_ga(b);
        pop[i].board = b;
        eval_individual(&pop[i]);
    }

    /* track absolute best */
    int best_cost = pop[0].cost, best_idx = 0;
    for (int i = 1; i < population_size; ++i)
        if (pop[i].cost < best_cost) { best_cost = pop[i].cost; best_idx = i; }

    /* ---------------- evolutionary loop ----------------------------------- */
    for (int gen = 0; gen < generations && best_cost; ++gen) {
        /* Elitism : copy best individual to next[0] */
        Board *elite_board = board_create(n);
        memcpy(elite_board->cells, pop[best_idx].board->cells, sizeof(int)*total);
        next[0].board = elite_board;
        next[0].cost  = pop[best_idx].cost;

        /* Fill rest of next population */
        for (int i = 1; i < population_size; ++i) {
            int p1 = tournament_select_ga(pop, population_size, 3);
            int p2 = tournament_select_ga(pop, population_size, 3);
            while (p2 == p1) p2 = rand() % population_size;

            Board *child_board = board_create(n);
            double r = (double)rand()/RAND_MAX;
            if (r < crossover_rate) {
                crossover_uniform_ga(pop[p1].board, pop[p2].board, child_board, fixed);
            } else {
                memcpy(child_board->cells, pop[p1].board->cells, sizeof(int)*total);
            }

            r = (double)rand()/RAND_MAX;
            if (r < mutation_rate) swap_in_block_ga(child_board, fixed);

            next[i].board = child_board;
            eval_individual(&next[i]);
        }

        /* swap populations */
        for (int i = 0; i < population_size; ++i) {
            board_destroy(pop[i].board);
            pop[i] = next[i];
        }

        /* find best in current population */
        best_cost = pop[0].cost; best_idx = 0;
        for (int i = 1; i < population_size; ++i)
            if (pop[i].cost < best_cost) { best_cost = pop[i].cost; best_idx = i; }
#ifdef VERBOSE_GA
        if (gen % 100 == 0)
            printf("gen %4d | best cost = %d\n", gen, best_cost);
#endif
    }

    memcpy(puzzle->cells, pop[best_idx].board->cells, sizeof(int)*total);

    for (int i = 0; i < population_size; ++i) board_destroy(pop[i].board);
    free(pop);
    free(next);
    free(fixed);
    return best_cost == 0;
}

    
