# ──────────────────────────────────────────────────────────────────
# Makefile — CodeOS Mini Operating System
# ──────────────────────────────────────────────────────────────────
# Build:  make
# Clean:  make clean
# Run:    make run  (or  ./codeos)
# ──────────────────────────────────────────────────────────────────

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Iinclude
TARGET  = Jarvis-miniOS

# Source directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = build

# Source files (main.c will link all others)
SRCS    = $(wildcard $(SRC_DIR)/*.c)
OBJS    = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# ── Default target ───────────────────────────────────────────────
all: $(OBJ_DIR) $(TARGET)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# ── Link all object files into the final binary ─────────────────
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# ── Compile each .c to .o ───────────────────────────────────────
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ── Run the OS ───────────────────────────────────────────────────
run: all
	./$(TARGET)

# ── Clean build artifacts ───────────────────────────────────────
clean:
	rm -rf $(OBJ_DIR) $(TARGET) codeos

# ── Dependencies ────────────────────────────────────────────────
$(OBJ_DIR)/string.o:   $(SRC_DIR)/string.c $(INC_DIR)/string.h
$(OBJ_DIR)/memory.o:   $(SRC_DIR)/memory.c $(INC_DIR)/memory.h $(INC_DIR)/string.h
$(OBJ_DIR)/math.o:     $(SRC_DIR)/math.c $(INC_DIR)/math.h
$(OBJ_DIR)/screen.o:   $(SRC_DIR)/screen.c $(INC_DIR)/screen.h $(INC_DIR)/string.h
$(OBJ_DIR)/keyboard.o: $(SRC_DIR)/keyboard.c $(INC_DIR)/keyboard.h
$(OBJ_DIR)/system.o:   $(SRC_DIR)/system.c $(INC_DIR)/system.h
$(OBJ_DIR)/ui.o:       $(SRC_DIR)/ui.c $(INC_DIR)/ui.h
$(OBJ_DIR)/process.o:  $(SRC_DIR)/process.c $(INC_DIR)/process.h
$(OBJ_DIR)/fs.o:       $(SRC_DIR)/fs.c $(INC_DIR)/fs.h
$(OBJ_DIR)/net.o:      $(SRC_DIR)/net.c $(INC_DIR)/net.h
$(OBJ_DIR)/main.o:     $(SRC_DIR)/main.c $(INC_DIR)/string.h $(INC_DIR)/memory.h $(INC_DIR)/math.h $(INC_DIR)/screen.h $(INC_DIR)/keyboard.h $(INC_DIR)/system.h $(INC_DIR)/ui.h $(INC_DIR)/process.h $(INC_DIR)/fs.h $(INC_DIR)/net.h

.PHONY: all clean run
