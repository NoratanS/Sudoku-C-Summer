# --- Makefile ---

# Compile and flags
CC      := gcc
CFLAGS  := -std=gnu11 -Wall -Wextra -pedantic -Iinclude
LDFLAGS := -lm

# Windows check
ifeq ($(OS),Windows_NT)
  EXEEXT    := .exe
  RM        := del /Q
else
  EXEEXT    :=
  RM        := rm -f
endif

TARGET   := sudoku$(EXEEXT)

SRC      := $(wildcard src/*.c)
OBJ      := $(patsubst src/%.c,%.o,$(SRC))

.PHONY: all run clean

all: $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

# Compile
%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# At make‐parse time pick the right invocation
ifeq ($(OS),Windows_NT)
  EXEC := $(TARGET)
else
  EXEC := ./$(TARGET)
endif

run: all
	$(EXEC)

clean:
	-$(RM) $(OBJ) $(TARGET)
