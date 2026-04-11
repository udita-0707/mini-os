# ──────────────────────────────────────────────────────────────────
# Makefile — CodeOS Mini Operating System
# ──────────────────────────────────────────────────────────────────
# Build:  make
# Clean:  make clean
# Run:    make run  (or  ./codeos)
# ──────────────────────────────────────────────────────────────────

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99
TARGET  = codeos

# Source files — order matches the OS layer hierarchy
SRCS    = string.c memory.c math.c screen.c keyboard.c main.c
OBJS    = $(SRCS:.c=.o)

# ── Default target ───────────────────────────────────────────────
all: $(TARGET)

# ── Link all object files into the final binary ─────────────────
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# ── Compile each .c to .o ───────────────────────────────────────
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ── Run the OS ───────────────────────────────────────────────────
run: $(TARGET)
	./$(TARGET)

# ── Clean build artifacts ───────────────────────────────────────
clean:
	rm -f $(OBJS) $(TARGET)

# ── Dependencies (manual — no makedepend needed) ────────────────
string.o:   string.c string.h
memory.o:   memory.c memory.h string.h
math.o:     math.c math.h
screen.o:   screen.c screen.h string.h
keyboard.o: keyboard.c keyboard.h
main.o:     main.c string.h memory.h math.h screen.h keyboard.h

.PHONY: all clean run
