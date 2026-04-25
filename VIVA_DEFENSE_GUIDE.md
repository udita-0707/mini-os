# Jarvis-miniOS Viva Defense & Capstone Guide

This handbook provides a systematic breakdown of the Jarvis-miniOS architecture, module-by-module walkthroughs, system design rationale, and an interview question bank designed to prepare you for your capstone project defense.

---

## 2. PROJECT WALKTHROUGH GUIDE (File-by-File Breakdown)

### `memory.c` & `memory.h`
- **Purpose**: Acts as the physical RAM layer, tracking a simulated 1MB byte array.
- **Key Details**: Uses a sequential bump-allocator or tracked block strategy. It maintains a `MemoryBlock` array to track active pointers, size, and ownership (PID). 
- **Edge Cases**: Out of memory (OOM) fragmentation, freeing an invalid index.

### `process.c` & `process.h`
- **Purpose**: Manages concurrent tasks using a cooperative round-robin scheduler.
- **Key Details**: `process_schedule()` runs once per 5ms tick, advancing the state of active background processes (like `counter`).

### `fs.c` & `fs.h`
- **Purpose**: A Virtual File System (VFS) that maps logical file names to physical memory addresses.
- **Key Details**: Maintains a file table storing metadata (name, size, permissions, memory pointer).

### `net.c` & `net.h`
- **Purpose**: Simulates a network stack by injecting artificial latency before processing messages.

### `screen.c` & `keyboard.c`
- **Purpose**: Hardware abstraction layers.
- **Key Details**: `keyboard.c` uses `termios.h` and `select()` to provide non-blocking input (`keyboard_get_char_nonblocking()`), which is critical for the cooperative scheduler to work. `screen.c` manages ANSI escape codes for cursor positioning and colored text.

### `system.c` & `system.h`
- **Purpose**: The System Call (Syscall) API. It provides a standardized wrapper around the lower-level modules.
- **Key Details**: Implements `trace mode`. If tracing is enabled, calls to `sys_print` or `sys_alloc` will log interception data before executing the underlying function.

### `ui.c` & `ui.h`
- **Purpose**: Handles high-level display logic, including the themed dashboard, footer, and animated boot sequences.

### `math.c` & `string.c`
- **Purpose**: Utility libraries built from scratch to avoid standard library dependencies.

### `main.c`
- **Purpose**: The entry point. Initializes all drivers, drops into the `shell_run()` infinite loop, and tokenizes user input to execute commands.

---

## 3. FUNCTION-BY-FUNCTION EXPLANATION

### `math.c`
- **`math_multiply(int a, int b)`**: Russian Peasant Multiplication (Shift-and-Add). Achieves O(log N) time by shifting bits rather than repeated addition.
- **`math_divide(int a, int b)` / `math_modulo(int a, int b)`**: Binary Long Division. Uses bit shifts to align the divisor, dropping time complexity from O(N) to O(log N).
- **`math_abs(int x)`**: Returns absolute value.
- **`math_min` / `math_max` / `math_clamp`**: Boundary and utility functions for UI and limits.
- **`math_power(int base, int exp)`**: Uses Exponentiation by Squaring for O(log N) efficiency.
- **`math_integer_sqrt(int n)`**: Bitwise binary search digit-by-digit algorithm for square roots.
- **`math_gcd` / `math_lcm`**: Euclidean algorithm implementation.
- **Bitwise Helpers** (`math_is_power_of_two`, `math_next_power_of_two`, `set_bit`, `clear_bit`, `toggle_bit`): Core utilities for bit manipulation.
- **`math_safe_add` / `math_safe_sub`**: Includes underflow/overflow detection via pointers.
- **`math_align_up` / `math_align_down`**: Bitwise alignment functions crucial for memory block boundary alignment.

### `string.c`
- **`str_length(const char *s)`**: Iterates until `\0` to return string length. Solves standard `strlen`.
- **`str_copy(char *dest, const char *src)`**: Deep copies character arrays. Essential because C cannot assign string literals to arrays directly.
- **`str_compare(const char *a, const char *b)`**: Iterates through both strings. Returns 0 if identical. Crucial for the shell command parser.
- **`str_starts_with(const char *s, const char *prefix)`**: Checks if a string begins with a prefix, useful for partial command matching.
- **`str_concat(char *dest, const char *src)`**: Appends `src` to `dest`. Used to build filesystem log strings.
- **`tokenize(const char *input, char tokens[][MAX_TOKEN_LEN], int max_tokens)`**: The hardest string function. It parses raw terminal input into arrays of arguments while respecting single quotes (`'hello world'`). Time complexity: O(N).
- **`int_to_string(int num, char *buffer)`**: Converts integers to characters via modulo 10 arithmetic.
- **`str_to_int(const char *s)`**: Reverses `int_to_string`. Solves `<stdlib.h>` `atoi`.

### `memory.c`
- **`mem_init(void)`**: Sets all 1MB `ram` to 0 and clears the `MemoryBlock` tracking array.
- **`mem_alloc(int size)`**: The core allocator. Finds an empty tracking block, verifies contiguous free space remains, updates the block metadata (start index, size), and returns the pointer. Fails if memory is exhausted. Time Complexity: O(N) over blocks.
- **`mem_free(int block_idx)`**: Takes a block ID and sets its `active` flag to 0, functionally freeing the space for future allocations.
- **`mem_get_info(int *used, int *total, int *blocks)`**: Calculates system health metrics for the dashboard by summing active block sizes.

### `screen.c`
- **`screen_init(void)`**: Clears terminal and sets initial configurations.
- **`screen_print(const char *text)`**: Writes raw strings to standard output via `write()`.
- **`screen_print_line(const char *text)`**: Prints string followed by `\r\n`.
- **`screen_print_int(int value)`**: Uses `int_to_string` to safely print numbers.
- **`screen_clear(void)`**: Sends the ANSI escape code `\033[2J\033[H` to clear the terminal window entirely.
- **`screen_newline(void)`**: Moves cursor to next line securely.
- **`screen_set_color(int fg, int bg)`**: Sends ANSI codes to alter foreground and background text colors dynamically.
- **`screen_reset_color(void)`**: Reverts to terminal default colors.
- **`screen_set_cursor(int row, int col)`**: Uses ANSI `\033[row;colH` to jump the cursor to exact coordinates for UI rendering.
- **`screen_save_cursor(void)` / `screen_restore_cursor(void)`**: ANSI macros to preserve cursor location before drawing UI elements.
- **`screen_hide_cursor(void)` / `screen_show_cursor(void)`**: ANSI macros to prevent cursor blinking during boot sequences.

### `keyboard.c`
- **`keyboard_init(void)`**: Crucial function. Modifies the POSIX `termios` flags to disable `ICANON` and `ECHO`, switching the terminal into raw mode.
- **`keyboard_shutdown(void)`**: Restores original `termios` settings before exit.
- **`keyboard_read_line(...)`**: A blocking input reader used in older phases.
- **`keyboard_key_pressed(void)`**: Uses POSIX `select()` to peek at the input buffer to see if a key is waiting, returning immediately.
- **`keyboard_get_char_nonblocking(void)`**: If `key_pressed` is true, reads the character; else returns -1. The foundation of the multitasking shell.

### `process.c`
- **`process_init(void)`**: Clears the process table.
- **`process_create(const char *name, void (*task_func)(void))`**: Registers a new function pointer in the scheduler table, assigns a PID, and marks it active.
- **`process_kill(int pid)`**: Marks a PID as inactive, effectively removing it from the execution loop.
- **`process_schedule(void)`**: Iterates the process table by 1 step. If the current index is active, it calls the `task_func()`. Operates on a round-robin cycle.
- **`process_get_count(void)`**: Returns the number of running processes for the UI dashboard.
- **`task_counter(void)`**: The logic for the `run counter` background task. Uses the VFS `fs_write` to log its state.
- **`task_snake(void)`**: The blocking foreground mini-game logic. 

### `fs.c`
- **`fs_init(void)`**: Clears the virtual file table.
- **`get_file_idx(const char *name)`**: Helper function to linearly search for a file by name.
- **`fs_touch(const char *name)`**: Creates an empty file entry in the table. Fails if the file exists or the table is full.
- **`fs_write(const char *name, const char *data)`**: Allocates memory via `sys_alloc` using the length of `data`. Updates the file's memory pointer and copies the string in. Fails if the file is marked read-only.
- **`fs_read(const char *name)`**: Looks up the file's memory pointer and prints it using `sys_print`.
- **`fs_rm(const char *name)`**: Calls `sys_free` on the file's memory pointer and clears the table entry.
- **`fs_chmod(const char *name, int permissions)`**: Modifies the file's read-write or read-only boolean flag.
- **`fs_ls(void)`**: Prints a formatted table of all active files, their sizes, and permissions.

### `net.c`
- **`net_init(void)`**: Initializes dummy host tables.
- **`get_host_idx(...)`**: Helper function to find network destinations.
- **`net_ping(const char *ip)`**: Simulates an ICMP echo request by calling `usleep()` multiple times based on the target's configured latency.
- **`net_send(const char *ip, const char *msg)`**: Simulates a data packet transfer, using latency loops before confirming delivery.

### `ui.c`
- **`ui_init(void)`**: Loads default color themes.
- **`ui_set_theme(const char *theme_name)`**: Switches global `current_theme` struct values between Hacker, Retro, and Ocean color palettes.
- **`ui_apply_theme(void)`**: Pushes the current theme colors to the `screen.c` hardware layer.
- **`print_animated(...)` / `print_loading_step(...)`**: Helpers to fake boot-up times using `usleep`.
- **`ui_boot_sequence(void)`**: Renders the falling-matrix ASCII "JARVIS" logo animation frame by frame.
- **`ui_launcher_menu(void)`**: Prints the 1-4 selection menu.
- **`ui_draw_progress_bar(...)`**: Mathematically converts a ratio into a visual block bar `[■■■□□□]`.
- **`ui_dashboard(void)`**: Clears the screen and draws an immersive system overview box utilizing the progress bar function.
- **`ui_draw_footer(...)`**: Inline status printer that updates the user on RAM, Procs, and Health directly above the shell prompt.

### `system.c`
- **`sys_set_trace(...)` / `sys_get_trace(...)`**: Toggle flags for the diagnostics engine.
- **`sys_print(...)`, `sys_print_line(...)`, `sys_print_int(...)`**: Wrap `screen.c` functions. If `trace == 1`, they intercept the output and print `[TRACE] string -> screen` before passing the data to hardware.
- **`sys_alloc(...)`, `sys_free(...)`**: Wrap `memory.c` allocator functions. Intercepted for tracing `fs -> memory` calls.
- **`sys_create_process`, `sys_kill_process`, `sys_touch`, `sys_rm`, `sys_ping`**: System call wrappers abstracting the internal subsystems away from `main.c`.

### `main.c`
- **`print_prompt(void)`**: Prints the formatted `guest@minios:~$` shell line.
- **`handle_command(char *input)`**: The massive IF/ELSE block. It receives the raw string, passes it to `tokenize`, and then routes the command to the respective system call (`sys_alloc`, `sys_ping`, `fs_ls`, etc).
- **`shell_run(void)`**: The architectural heartbeat of the OS. An infinite `while(running)` loop that:
  1. Calls `keyboard_get_char_nonblocking()`.
  2. Evaluates the backspace buffer or passes full commands to `handle_command()`.
  3. Calls `ui_draw_footer()` prior to reprompting.
  4. Calls `process_schedule()` to advance background threads.
  5. Uses `usleep(5000)` to govern the CPU cycle speed.
- **`main(void)`**: Initializes all OS modules in strict order (screen, keyboard, memory, net, process, fs, ui) before calling `ui_boot_sequence()` and transferring control to `shell_run()`.

---

## 4. PIPELINE EXPLANATION

**Command Flow Pipeline**: `keyboard -> string -> memory -> math -> screen`

1. **Input (keyboard)**: `keyboard_get_char_nonblocking()` registers a physical keystroke.
2. **Parsing (string)**: The shell buffer accumulates the characters. When 'Enter' is pressed, custom `string.c` functions (`str_split`, `str_compare`) parse the input into a command and arguments.
3. **Execution (memory/fs)**: If the command is `write`, the Virtual FS is invoked, which calculates size and calls `sys_alloc` (`memory.c`) to reserve a RAM block.
4. **Processing (math)**: For diagnostic commands or progress bars, `math.c` calculates percentages (e.g., `(used * 100) / total`).
5. **Output (screen)**: `sys_print` sends the formatted ANSI string to `screen.c` to render on the terminal.

---

## 5. VIVA QUESTION BANK

### Beginner
**Q: Why did you build custom libraries instead of using `<string.h>` and `malloc`?**
A: To demonstrate a foundational understanding of operating systems. A real kernel cannot rely on a standard C library because it *provides* the environment that the C library runs on.

**Q: Why use Virtual RAM instead of writing directly to the host's physical RAM?**
A: Operating systems isolate processes using virtual memory. In our user-space simulation, we mapped a 1MB byte array to represent physical RAM, demonstrating allocation logic without risking segmentation faults on the host OS.

### Intermediate
**Q: Explain your memory allocator strategy.**
A: It utilizes a simplified sequential allocation strategy (bump allocator) tracked by a block table. It tracks the start index, size, and PID owner. While basic, it clearly demonstrates the lifecycle of memory.

**Q: How does the scheduler work?**
A: It is a cooperative round-robin scheduler. The main loop calls `process_schedule()` every 5ms tick. The scheduler iterates through the active process table and executes the function pointer of the next task, relying on the task to quickly return control (yield).

**Q: How does the tokenizer handle multi-word strings?**
A: It scans for spaces to delimit tokens but uses a state flag to detect single quotes. If inside quotes, spaces are ignored, treating the entire phrase as one token.

**Q: How did you optimize the math operations without a math library?**
A: I replaced naive O(N) loop-based operations with O(log N) bitwise algorithms. For multiplication, I used Russian Peasant Multiplication (Shift-and-Add). For division and modulo, I implemented Binary Long Division using bit shifts to align the divisor. I also implemented Exponentiation by Squaring for power functions and the Euclidean algorithm for GCD.

### Advanced
**Q: What breaks under memory fragmentation in your system?**
A: Because we use a basic allocator, if we allocate Block A, B, and C, and then free Block B, that space might not be reusable if the next allocation is larger than B's size. Over time, the RAM array will fill up with gaps, leading to an Out of Memory error even if total free space is sufficient. Paging or a linked-list free-block coalescing strategy would fix this.

**Q: Why use cooperative scheduling over preemptive scheduling?**
A: Preemptive scheduling requires hardware timer interrupts to forcefully pause a thread and save its CPU registers (context switching). In a purely C-based user-space simulation, we cannot easily manipulate CPU registers safely, making cooperative (yield-based) scheduling the only viable software-level approach.

**Q: What are the security limitations of this OS?**
A: There is no strict ring-protection. A process can technically write to any index in the 1MB RAM array, easily overwriting the Virtual FS data of another process.

---

## 6. SYSTEM DESIGN EXPLANATION

- **Process Management**: Chosen for simplicity, the cooperative model allows us to simulate background tasks (like `counter`) running concurrently with user input, without dealing with POSIX threads or mutex locking.
- **File System Model**: Implemented as an in-memory Virtual FS (VFS). It proves that files are simply metadata tables pointing to memory blocks. 
- **System Call Abstraction**: By funneling all module interactions through `system.c`, we achieved a loosely coupled architecture. This made it trivial to implement `trace mode`, acting like `strace` in Linux to monitor all data passing between user space and kernel space.

---

## 7. DEMO WALKTHROUGH SCRIPT

To impress the examiner, execute these commands in this exact sequence:

1. **Boot**: Run `./Jarvis-miniOS`. (Point out the animated falling matrix boot sequence).
2. **Dashboard**: Type `dashboard`. (Explain the persistent UI rendering).
3. **Memory Lifecycle**: 
   - Type `meminfo` (Show it's empty).
   - Type `memtest "Defending my capstone!"` (Simulate an allocation).
   - Type `meminfo` (Show the block is tracked).
   - Type `free 0` and `meminfo` (Show deallocation).
4. **File System**:
   - `touch secret.txt`
   - `write secret.txt 'Hello Examiner'`
   - `chmod secret.txt 0` (Set to read-only).
   - `write secret.txt 'Hacked'` (Show permission denied error).
   - `read secret.txt` (Show it's intact).
5. **Multitasking**:
   - `run counter`
   - `top` (Show it running in the background).
   - `read counter.log` (Show it writing to the VFS concurrently).
6. **Tracing**:
   - `trace on`
   - `echo test` (Show the system call interception).
   - `trace off`
7. **UI**:
   - `theme hacker` -> `theme ocean`.

---

## 8. UNIQUE PROJECT DIFFERENTIATORS

If asked "What makes this project special compared to a standard CLI app?":
1. **No Standard Libs**: It reinvents the wheel on purpose, utilizing custom math, string, and memory libraries to prove fundamental understanding.
2. **Algorithmic Bitwise Math**: Operations like multiplication and division are optimized down to O(log N) time using hardware-level bit shifts (e.g., Russian Peasant Multiplication).
3. **Manual Memory Lifecycle**: It visually exposes memory allocation (`meminfo`) rather than hiding it behind abstractions.
4. **Trace Mode**: The built-in syscall interception allows you to visualize the data pipeline in real-time, an advanced diagnostic feature.
5. **Non-blocking I/O**: It uses pure C `termios` configuration to achieve a highly responsive shell that can handle background tasks without thread locking.

---

## 9. TROUBLESHOOTING SECTION

If you run into issues during your defense, explain them confidently:

- **Segmentation Faults**: Usually caused by a pointer pointing outside the 1MB `ram` array bounds. Explain that without an MMU (Memory Management Unit), out-of-bounds writes directly crash the host process.
- **Garbage Output in Files**: Caused by stale pointers. If a file is freed but not removed from the `fs.c` table, `read` will print whatever new data was allocated over that memory space (a classic use-after-free bug).
- **Frozen Terminal**: If a process enters an infinite loop (`while(1)`), the cooperative scheduler cannot regain control. Explain that this is a known limitation of cooperative multitasking, solved in modern OSs via hardware preemptive interrupts.
