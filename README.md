# CodeOS — Jack OS-Style Mini Operating System

A simulated operating system built entirely in C, running as a terminal application. Implements core OS concepts — virtual memory management, custom string/math libraries, and a device-abstracted I/O layer — all without `<string.h>`, `<math.h>`, or built-in `malloc`/`free`.

## Architecture

```
┌──────────────────────────────────┐
│     Shell (main.c)               │  ← Interactive command loop
├──────────────────────────────────┤
│     Custom OS Libraries          │  ← string, memory, math
├──────────────────────────────────┤
│     Virtual Hardware Layer       │  ← screen, keyboard
└──────────────────────────────────┘

Pipeline: keyboard → string → memory → math → screen
```

## Features

- **Custom String Library** — `str_length`, `str_copy`, `str_compare`, `tokenize`, `int_to_string`, `str_to_int`
- **Virtual Memory Manager** — 1 MB virtual RAM with first-fit allocator, block splitting & coalescing
- **Custom Math Library** — `multiply`, `divide`, `modulo`, `abs` via loops (no `<math.h>`)
- **Device-Abstracted I/O** — Screen driver with cursor tracking, keyboard driver wrapping raw input
- **Interactive Shell** — 8 built-in commands with error handling
- **Command History** — Circular buffer storing last 10 commands

## Quick Start

### Build
```bash
make
```

### Run
```bash
./codeos
```

### Clean
```bash
make clean
```

## Shell Commands

| Command         | Description                           |
|-----------------|---------------------------------------|
| `help`          | Show available commands               |
| `echo <text>`   | Print text (allocates in virtual RAM) |
| `clear`         | Clear the terminal screen             |
| `about`         | Display system information            |
| `meminfo`       | Show memory allocation table          |
| `memtest`       | Run alloc → write → read → free test |
| `history`       | Show last 10 commands                 |
| `exit`          | Shutdown CodeOS                       |

## Example Session

```
CodeOS > help
  ╔═══════════════════════════════════════════╗
  ║          CodeOS Command Reference         ║
  ╠═══════════════════════════════════════════╣
  ║  help           Show this help message    ║
  ║  echo <text>    Print text to screen      ║
  ║  clear          Clear the screen          ║
  ║  about          System information        ║
  ║  meminfo        Memory allocation table   ║
  ║  memtest        Allocate + free test      ║
  ║  history        Show command history      ║
  ║  exit           Shutdown CodeOS           ║
  ╚═══════════════════════════════════════════╝

CodeOS > echo Hello World
  Hello World

CodeOS > memtest
  [memtest] Starting memory test...
  [memtest] Allocated 64 bytes
  [memtest] Wrote: Hello from virtual RAM!
  [memtest] Memory used: 64 bytes
  [memtest] Block freed
  [memtest] Memory used after free: 0 bytes
  [memtest] ✓ Memory test passed!

CodeOS > exit
  Shutting down CodeOS...
  Freeing virtual RAM...
  Goodbye!
```

## Project Structure

```
mini-os/
├── string.h / string.c       # Custom string operations
├── memory.h / memory.c       # Virtual RAM & allocator
├── math.h   / math.c         # Math without <math.h>
├── screen.h / screen.c       # Screen output driver
├── keyboard.h / keyboard.c   # Keyboard input driver
├── main.c                    # Boot sequence & shell
├── Makefile                  # Build system
└── README.md
```

## Constraints

| Rule | Detail |
|------|--------|
| ❌ `<string.h>` | All string ops hand-written |
| ❌ `<math.h>` | Multiply/divide via loops |
| ❌ `malloc`/`free` | Custom allocator on virtual RAM |
| ✅ `<stdio.h>` | Used only in screen/keyboard drivers |
| ✅ `<stdlib.h>` | Used **once** to allocate the virtual RAM block |

## Build Requirements

- **GCC** (or any C99 compiler)
- Compiles cleanly with `-Wall -Wextra -std=c99`

## Phase 1 Status

This is Phase 1 of the CodeOS project. Future phases will add:
- Virtual file system
- Process management & cooperative scheduling
- System call layer