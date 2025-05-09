#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "board.h"

#define VERBOSE_SA 1    // 1 - prints progress info, 0 - doesn't xd

// Fisher-Yates shuggle for an integer array
static void shuffle_int(int *arr, int n) {
  for (int i = n - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    int t=arr[i];
    arr[i] = arr[j];
    arr[j] = t;
  }
}

// Calculates the cost (number of row/column conflicts) of the board
static int calculate_cost(const Board* board) {
  int n = board->size;
  int cost = 0;
  int count[n+1];

  // rows
  for (int row = 0; row < n; row++) {
    memset(count, 0, sizeof(count));
    for (int col = 0; col < n; col++) {
      int val = board_get(board, row, col);
      if (val > 0) {
        count[val]++;
      }
    }
    for (int val = 1; val <= n; val++) {
      if (count[val] > 1) {
        cost += count[val] - 1;
      }
    }
  }

   // columns
   for (int col = 0; col < n; col++) {
     memset(count, 0, sizeof(count));
     for (int row = 0; row < n; row++) {
       int val = board_get(board, row, col);
       if (val > 0) {
         count[val]++;
       }
     }
     for (int val = 1; val <= n; val++) {
       if (count[val] > 1) {
         cost += count[val] - 1;
       }
     }
   }
  return cost;
}

// Fill all empty cells in each 3×3 block with the missing digits
// so that each block contains all values 1..n exactly once.
static void random_fill_blocks(Board* board) {
  int n = board->size;
  int block_size = (int) sqrt(n);

  int missing[n];  // up to n empty numbers
  int miss_count;  // flat indices of empty (non-fixed) cells
  int empty[n];  // what cells are empty
  int empty_count; // number of empty cells collected

  for(int br = 0; br < block_size; br++) {
    for (int bc = 0; bc < block_size; bc++) {
      miss_count = 0;
      empty_count = 0;
      bool present[17] = {0}; // cant be variable sized, but also we can't have more than 16 numbers LOL!
      // mark present values and collect empty cell indicies
      for (int dr = 0; dr < block_size; dr++) {
        for (int dc = 0; dc < block_size; dc++) {
          int row = br * block_size + dr;
          int col = bc * block_size + dc;
          int val = board_get(board, row, col);

          // mark
          if (val > 0) {
            present[val] = true;
          }
          // collect
          else {
            empty[empty_count++] = row * n+ col;  // board[row, col] is empty!!!
          }
        }
      }

      // prep missing
      for (int val = 1; val <= n; val++) {
        if (!present[val]) {
          missing[miss_count++] = val;
        }
      }
      // shuffle and fill
      shuffle_int(missing, miss_count);
      for (int i = 0; i < miss_count; i++) {
        int idx = empty[i];
        int row = idx / n;
        int col = idx % n;
        board_set(board, row, col, missing[i]);
      }
    }
  }
}

// swap two non-fixed cells with random block, returns true if successful
static bool neighbour_swap(Board* board, const bool* fixed) {
  int n = board->size;
  int block_size = (int) sqrt(n);
  int tries = 0;
  while (tries < block_size + 1) {
    int br = rand() % block_size;
    int bc = rand() % block_size;
    int candidates[n];
    int count = 0;

    // Collect indices of non-fixed cells in this block
    for (int dr = 0; dr < block_size; dr++) {
      for (int dc = 0; dc < block_size; dc++) {
        int row = br * block_size + dr;
        int col = bc * block_size + dc;
        int idx = row * n + col;
        if (!fixed[idx]) {
          candidates[count++] = idx;
        }
      }
    }
    if (count < 2) {
      tries++;
      continue;
    }

    // Pick and swap 2
    int i = rand() % count;
    int j;
    do {
      j = rand() % count;
    } while (j==i);
    int idx1 = candidates[i];
    int idx2 = candidates[j];
    int row1 = idx1 / n;
    int col1 = idx1 % n;
    int row2 = idx2 / n;
    int col2 = idx2 % n;
    int val1 = board_get(board, row1, col1);
    int val2 = board_get(board, row2, col2);
    // perform the swap
    board_set(board, row1, col1, val2);
    board_set(board, row2, col2, val1);
    return true;
  }
  return false; // no movable block found
}

bool solver_sa(Board* puzzle, double t_start, double t_end, double alpha, int max_iters) {
  if (!puzzle) return false;
  srand((unsigned)time(NULL));
  int n = puzzle->size;
  const int total = n * n;

  // fixed mask
  bool fixed[total];
  for (int i = 0; i < total; i++) {
    fixed[i] = puzzle->cells[i] != 0;
  }

  // create working board copy
  Board* curr = board_create(n);
  memcpy(curr->cells, puzzle->cells, sizeof(int) * total);
  random_fill_blocks(curr);
  int curr_cost = calculate_cost(curr);

  // keep best solution
  Board* best = board_create(n);
  memcpy(best->cells, curr->cells, sizeof(int) * total);
  int best_cost = curr_cost;

  // store backup if swap fails
  Board* backup = board_create(n);

#if VERBOSE_SA
  printf("iter\tT\t\tbest\n");
#endif

  double T = t_start;
  for (int iter = 0; iter < max_iters && best_cost > 0; iter++) {
    // keep current state backed up
    memcpy(backup->cells, curr->cells, sizeof(int) * total);

    // attempt a neighbor move
    if (!neighbour_swap(curr, fixed)) {
      T *= alpha;
      if (T<t_end) T = t_end;
      continue;
    }
    int new_cost = calculate_cost(curr);
    int delta = new_cost - curr_cost;
    // Acceptance
    bool accept = false;
    if (delta <= 0) accept = true;
    else {
      double prob = exp(-(double) delta / T);
      if (((double)rand()/RAND_MAX) < prob) accept = true;
    }
    if (accept) {
      curr_cost = new_cost;
      if (curr_cost < best_cost) {
        best_cost = curr_cost;
        memcpy(best->cells, curr->cells, sizeof(int) * total);
      }
    }
    else {
      // undo swap
      memcpy(curr->cells, backup->cells, sizeof(int) * total);
    }
#if VERBOSE_SA
    if (iter % 1000 == 0)
      printf("%d\t%.6f\t%d\n", iter, T, best_cost);
#endif

    // cooling schedule
    T *= alpha;
    if (T<t_end) T = t_end;
  }

  // write best back to puzzle
  memcpy(puzzle->cells, best->cells, sizeof(int) * total);
  // cleanup
  board_destroy(curr);
  board_destroy(backup);
  board_destroy(best);
  return (best_cost == 0);
}