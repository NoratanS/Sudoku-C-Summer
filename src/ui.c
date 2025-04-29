#include "ui.h"
#include "generator.h"
#include "solver.h"
#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void read_line(char* buf, size_t sz) {
    if (fgets(buf, sz, stdin)) {
        buf[strcspn(buf, "\r\n")] = '\0';
    }
}

// Generates new game based on player preferences
static void new_game(GameState* gs) {
    char buf[64];

    // size
    do {
        printf("Select board size (4,9,16): ");
        read_line(buf,sizeof buf);
        gs->size = atoi(buf);
    } while(gs->size!=4 && gs->size!=9 && gs->size!=16);

    // hints
    printf("Enter hints allowed: ");
    read_line(buf,sizeof buf);
    gs->hints = atoi(buf);

    // difficulty
    do {
        printf("Select difficulty (1-E,2-M,3-H): ");
        read_line(buf,sizeof buf);
        gs->difficulty = atoi(buf);
    } while(gs->difficulty<1||gs->difficulty>3);

    if (gs->board) board_destroy(gs->board);
    gs->board = generator_full_board(gs->size);
    int total = gs->size * gs->size;
    int removeCount;
    switch (gs->difficulty) {
        case 1: removeCount = total * 4 / 10; break;
        case 2: removeCount = total * 6 / 10; break;
        case 3: removeCount = total * 8 / 10; break;
        default: removeCount = total * 6 / 10;
    }
    generator_remove_cells(gs->board, removeCount);
    gs->moves = 0;
    gs->start_time = time(NULL);
}

// User move
static void handle_move(GameState* gs, const char* cell, int val) {
    int r,c;
    if (!board_label_to_indices(gs->board, cell, &r, &c)) {
        printf("Invalid cell '%s'", cell);
        return;
    }
    if (val < 0 || val > gs->size) {
        printf("Invalid value '%d'", val);
        return;
    }
    if (val && !board_is_valid_move(gs->board,r,c,val)) {
        printf("Move violates rules");
        return;
    }
    board_set(gs->board,r,c,val);
    gs->moves++;
}

// Hint
static void show_hint(GameState* gs) {
    if (gs->hints <= 0) {
        printf("No hints left\n");
        return;
    }
    Board* copy = board_create(gs->size);
    memcpy(copy->cells, gs->board->cells, gs->size*gs->size*sizeof(int));
    solver_solve(copy);
    for (int r=0;r<gs->size;r++) for(int c=0;c<gs->size;c++) {
        if (!board_get(gs->board,r,c)) {
            printf("Hint %c%d = %d\n", 'A'+c, r+1, board_get(copy,r,c));
            gs->hints--;
            board_destroy(copy);
            return;
        }
    }
    board_destroy(copy);
    printf("No hints available\n");
}

// Saves game state to path
static void save_game(const GameState* gs, const char* path) {
    FILE* f = fopen(path, "w");
    if (!f) { perror("Save"); return; }
    // format: size hints difficulty moves elapsed
    fprintf(f, "%d %d %d %d %ld ",
            gs->size, gs->hints, gs->difficulty,
            gs->moves,
            (long)(time(NULL) - gs->start_time));
    // board
    for (int r = 0; r < gs->size; r++) {
        for (int c = 0; c < gs->size; c++) {
            fprintf(f, "%d ", board_get(gs->board, r, c));
        }
        fprintf(f, "");
    }
    fclose(f);
    printf("Game saved to %s", path);
}

// Loades game state from file
static void load_game(GameState* gs, const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) { perror("Load"); return; }
    int size, hints, difficulty, moves;
    long elapsed;
    if (fscanf(f, "%d %d %d %d %ld", &size, &hints, &difficulty, &moves, &elapsed) != 5) {
        printf("Invalid save file");
        fclose(f);
        return;
    }
    Board* b = board_create(size);
    for (int r = 0; r < size; r++) {
        for (int c = 0; c < size; c++) {
            int v; fscanf(f, "%d", &v);
            board_set(b, r, c, v);
        }
    }
    fclose(f);
    // replace game state
    if (gs->board) board_destroy(gs->board);
    gs->board = b;
    gs->size = size;
    gs->hints = hints;
    gs->difficulty = difficulty;
    gs->moves = moves;
    gs->start_time = time(NULL) - elapsed;
    printf("Game loaded from %s", path);
}

void ui_start(void) {
    GameState gs = {0};
    char buf[64];
    printf("Commands:\n i - show instructions\n n - new game\n m - move (m A1 [v])\n h - hint\n s - save (s <path>)\n l - load (l <path>)\n q - quit\n");


    // main loop
    while(1) {
        if (gs.board) {
            printf("%dx%d Difficulty:%d Hints:%d Moves:%d Time:%lds\n",
                  gs.size,gs.size, gs.difficulty, gs.hints, gs.moves,(long)(time(NULL)-gs.start_time));
        }
        board_print(gs.board);
        read_line(buf,sizeof buf);
        if(buf[0]=='q') break;
        if(buf[0]=='n') { new_game(&gs); continue; }
        if(buf[0]=='i') {
            printf("Commands:\n i - show instructions\n n - new game\n m - move (m A1 [v])\n h - hint\n s - save (s <path>)\n l - load (l <path>)\n q - quit\n");
            continue;
        }
        if(buf[0]=='m') {
            char cell[8];int v=0;int n=sscanf(buf+1," %7s %d",cell,&v);
            if(n>=1) {
                handle_move(&gs,cell,v);
                if(board_is_complete(gs.board)) {
                    printf("Congrats! Moves:%d Time:%lds",gs.moves,(long)(time(NULL)-gs.start_time));
                    break;
                }
            } else printf("Usage: m <cell> [value]");
            continue;
        }
        if(buf[0]=='h') { show_hint(&gs); continue; }
        if(buf[0]=='s'&&buf[1]==' ') { save_game(&gs,buf+2); continue; }
        if(buf[0]=='l'&&buf[1]==' ') { load_game(&gs,buf+2); continue; }
        printf("Unknown command");
    }
    board_destroy(gs.board);
    printf("\nGoodbye!");
}
