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
| `memtest <text>`| Allocate memory & store text          |
| `free`          | Free last allocation                  |
| `free <index>`  | Free specific allocation block        |
| `calc <a> op <b>`| Test math library (+ - * / %)         |
| `history`       | Show last 10 commands                 |
| `exit`          | Shutdown CodeOS                       |

## Example Session

```
CodeOS > help
  ╔═══════════════════════════════════════════╗
  ║          CodeOS Command Reference         ║
  ╠═══════════════════════════════════════════╣
  ║  help             Show this help message  ║
  ║  echo <text>      Print text to screen    ║
  ║  clear            Clear the screen        ║
  ║  about            System information      ║
  ║  meminfo          Memory allocation table ║
  ║  memtest <text>   Allocate & store text   ║
  ║  free             Free last allocation    ║
  ║  free <index>     Free specific block     ║
  ║  calc <a> <op> <b> Test math library      ║
  ║  history          Show command history    ║
  ║  exit             Shutdown CodeOS         ║
  ╚═══════════════════════════════════════════╝

CodeOS > echo Hello World
  Hello World

CodeOS > memtest Hello OS
  [memtest] Starting dynamic memory test...
  [memtest] Input: "Hello OS"
  [memtest] String length: 8 chars
  [memtest] Allocated 9 bytes in virtual RAM
  [memtest] Stored: Hello OS
  [memtest] Tracked as alloc #0
  [memtest] Memory NOT freed (use 'free' command)

CodeOS > free
  [free] Freeing alloc #0 ("Hello OS")
  [free] Releasing 9 bytes
  [free] Memory freed successfully
  [free] Memory used: 0 bytes

CodeOS > exit
  Shutting down CodeOS...
  Freeing virtual RAM...
  Goodbye!
```

https://github.com/user-attachments/assets/28d7784f-5fff-4e3a-9bf8-8512244de1e2



## Project Structure

```
mini-os/
├── src/                      # Source files
│   ├── string.c
│   ├── memory.c
│   ├── math.c
│   ├── screen.c
│   ├── keyboard.c
│   └── main.c
├── include/                  # Header files
│   ├── string.h
│   ├── memory.h
│   ├── math.h
│   ├── screen.h
│   └── keyboard.h
├── build/                    # Compiled object files (generated)
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
