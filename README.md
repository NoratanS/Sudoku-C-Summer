# Sudoku-C

A console-based Sudoku game written in C as an assignment for C programming course at University of Economics in Katowice, featuring:

- **Multiple board sizes**: 4×4, 9×9, 16×16  
- **Customizable game parameters**: number of hints allowed and difficulty level  
- **Generator**: backtracking generator that ensures a unique solution  
- **Interactive console UI**: new game, move input, hints, save/load, timer, move counter  
- **Portable build**: works with GNU Make or CMake on Linux/macOS/Windows (MinGW)

---

## Features

- **Board generation**  
  - Generates a fully filled valid Sudoku  
  - Removes cells according to chosen difficulty (40%/60%/80%)  
  - Guarantees exactly one solution

- **User interface**  
  - `n` – start a new game: choose size, hints allowed, difficulty  
  - `m <cell> [value]` – place a digit (e.g. `m A1 5`) or remove it (`m A1`)  
  - `h` – use a hint (fills one empty cell)  
  - `s <path>` – save current game state to file  
  - `l <path>` – load a saved game  
  - `i` – show command list / instructions  
  - `q` – quit the game

- **Game statistics**  
  - Move counter  
  - Elapsed time in seconds  

- **Save/load format**  
  - Saves board size, difficulty, hints left, move count, elapsed time  
  - Then the grid as comma-separated values, one row per line  
  - Easy to edit or inspect with a text editor

---

## Building & Running

### Prerequisites

- **GNU Make** (or `mingw32-make` on Windows/MinGW)  
- **GCC** or **Clang** (supports C11)  
- **CMake** (optional; if you prefer a CMake-based workflow)  

### 1. Using Make (recommended)

From the project root (where `Makefile` lives):

```bash
# Build everything
make

# Run the game
make run

# Clean up object files and the binary
make clean
```

Windows
```bash
# Build everything
mingw32-make

# Run the game
mingw32-make run

# Clean up object files and the binary
mingw32-make clean
```

## Issues
- Players can remove values from starting cells
- Because we use a solver to generate the board, 16x16 board works only with easy mode, unless you have a NASA PC lol.

## TODO 
- Fix the issues XD
- Make the board more readable, especially for 16x16

## Demo
![Sudoku-C-Tutorial](https://github.com/user-attachments/assets/1c97c361-7721-43ce-88b6-073fb6c6f6ca)

## There is no meme Łukasz only a bread pic 

![bread-pic](https://github.com/user-attachments/assets/e4e341ae-cc66-4cf4-829d-eaec0a447300)
