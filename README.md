# ./ Jarvis-miniOS

## Project Overview
Jarvis-miniOS is a Jack-style simulated mini operating system written entirely in C. It is designed to showcase the fundamental concepts of OS architecture, including manual memory management, cooperative process scheduling, virtual file systems, and a layered system call API, all without relying on standard C libraries like `<string.h>` or `<math.h>`. It provides a highly interactive, themed command-line interface.

## Core Objective
The primary objective of this project is to build a fully functional, simulated OS environment from scratch. It demonstrates a deep understanding of low-level system programming, memory allocation strategies, and modular layered architecture suitable for a capstone engineering project.

## Architecture Diagram (Layered OS Design)
```text
  [ USER SPACE ]
  +-------------------------------------------------------------+
  |  Shell Interface (main.c) | Dashboard (ui.c) | Games (proc) |
  +-------------------------------------------------------------+

  [ SYSTEM CALL LAYER ]
  +-------------------------------------------------------------+
  |  system.c (sys_alloc, sys_print, sys_create_process)        |
  +-------------------------------------------------------------+

  [ KERNEL & SUBSYSTEMS ]
  +---------------+  +---------------+  +-------+  +------------+
  | Process Sched |  | Virtual FS    |  | Net   |  | UI/Theme   |
  | (process.c)   |  | (fs.c)        |  | (net) |  | (ui.c)     |
  +---------------+  +---------------+  +-------+  +------------+

  [ CUSTOM HARDWARE ABSTRACTION & UTILS ]
  +----------+ +-----------+ +-----------+ +----------+ +-------+
  | keyboard | | string.c  | | memory.c  | | math.c   | | screen|
  +----------+ +-----------+ +-----------+ +----------+ +-------+
```

## Module Breakdown
1. **Hardware Abstraction**: `screen.c` and `keyboard.c` handle all non-blocking I/O and ANSI cursor manipulation.
2. **Utilities**: `string.c` and `math.c` provide custom implementations of standard library functions.
3. **Memory Manager**: `memory.c` carves up a 1MB virtual RAM block into tracked allocations.
4. **System Call Layer**: `system.c` provides wrapper APIs and a `trace` mode for monitoring cross-module communication.
5. **Subsystems**:
   - `process.c`: Cooperative round-robin scheduler.
   - `fs.c`: In-RAM virtual filesystem tracking metadata and data pointers.
   - `net.c`: Latency-simulating network router.
   - `ui.c`: Theme management and animated boot sequences.

## Feature List
- **Themed UI**: Animated falling-matrix boot sequence, persistent status footer, and custom color themes (Hacker, Retro, Ocean).
- **Manual Memory Lifecycle**: `memtest` to allocate memory, `meminfo` to view the allocation table, and `free` to deallocate.
- **Process Scheduler**: Cooperative multitasking allowing background jobs (`counter`) and foreground games (`snake`) to run simultaneously with the shell.
- **Virtual File System**: File creation, string writing, reading, deletion, and read-only permission toggling.
- **Diagnostics**: `trace mode` to intercept and log data flow across the pipeline.

## Build Instructions
Ensure you have `gcc` and `make` installed on your system.
```bash
make clean
make
```

## Run Instructions
Run the compiled executable from your terminal:
```bash
./Jarvis-miniOS
```

## Command Reference
| Category | Commands |
|----------|----------|
| **SYSTEM** | `help`, `about`, `history`, `clear`, `exit` |
| **MEMORY** | `meminfo`, `memtest <txt>`, `free <idx>` |
| **PROCESS** | `run counter`, `run snake`, `ps`, `top`, `kill <pid>` |
| **FILE** | `touch <f>`, `write <f> <d>`, `read <f>`, `rm <f>`, `ls`, `chmod <f> <perms>` |
| **NETWORK** | `ping <ip>`, `send <ip> <msg>` |
| **UI** | `theme <t>`, `dashboard` |
| **UTILS** | `echo <txt>`, `calc <a> <op> <b>`, `trace on/off` |
| **USERS** | `login <user>`, `whoami` |

## Example Usage
```bash
guest@minios:~$ memtest "Hello World"
guest@minios:~$ meminfo
guest@minios:~$ run counter
guest@minios:~$ read counter.log
guest@minios:~$ theme ocean
```

## Known Limitations
- The cooperative scheduler requires tasks to yield or complete quickly; a blocking `while(1)` in a task will freeze the OS.
- Virtual RAM is strictly limited to 1MB and does not support paging or swapping to disk.
- Filesystem data is entirely volatile and resets upon OS shutdown.

## Future Improvements
- **Preemptive Scheduling**: Integrating hardware timer interrupts to force context switching.
- **Disk Persistence**: Serializing the virtual filesystem to a local file on the host OS.
- **Paging**: Implementing a virtual-to-physical memory page table.
