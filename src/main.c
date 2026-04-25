/*
 * main.c — CodeOS Shell & Boot Sequence
 * ========================================
 * Entry point for the Mini OS simulation.
 *
 * Boot order:
 *   1. Initialize virtual RAM (memory.c)
 *   2. Initialize screen (screen.c)
 *   3. Print boot banner
 *   4. Enter interactive shell loop
 *   5. Shutdown on exit
 *
 * Pipeline enforced:
 *   keyboard → string → memory → math → screen
 *
 * Supported commands:
 *   help, echo, clear, about, memtest, meminfo, history, exit
 */

#include "keyboard.h"
#include "math.h"
#include "memory.h"
#include "screen.h"
#include "string.h"
#include "system.h"
#include "ui.h"
#include "process.h"
#include "fs.h"
#include "net.h"
#include <stdio.h>
#include <unistd.h>

/* ══════════════════════════════════════════════════════════════════ */
/*                       CONSTANTS                                  */
/* ══════════════════════════════════════════════════════════════════ */

#define INPUT_BUFFER_SIZE 256
#define HISTORY_SIZE      10
#define VERSION           "2.0"
#define MAX_ALLOCS        16    /* Max tracked manual allocations */

static char current_user[32] = "guest";

/* ══════════════════════════════════════════════════════════════════ */
/*                 ALLOCATION TRACKER                                */
/* ══════════════════════════════════════════════════════════════════ */

/*
 * Tracks pointers from memtest so the user can free them manually.
 * Each slot stores: pointer, size, and a label (first 31 chars).
 */
static char  *alloc_ptrs[MAX_ALLOCS];
static int    alloc_sizes[MAX_ALLOCS];
static char   alloc_labels[MAX_ALLOCS][32];
static int    alloc_count = 0;

/* ══════════════════════════════════════════════════════════════════ */
/*                     COMMAND HISTORY                               */
/* ══════════════════════════════════════════════════════════════════ */

/* Circular buffer for command history — stored in virtual RAM */
static char history[HISTORY_SIZE][INPUT_BUFFER_SIZE];
static int history_count = 0;
static int history_index = 0;

/*
 * history_add — Stores a command in the circular history buffer.
 */
static void history_add(const char *cmd) {
  /* Skip empty commands */
  if (str_length(cmd) == 0)
    return;

  str_copy(history[history_index], cmd);
  history_index = math_modulo(history_index + 1, HISTORY_SIZE);
  if (history_count < HISTORY_SIZE) {
    history_count++;
  }
}

/*
 * history_print — Displays the command history.
 */
static void history_print(void) {
  if (history_count == 0) {
    screen_print_line("  (no commands in history)");
    return;
  }

  screen_print_line("  Command History:");
  screen_print_line("  ────────────────");

  int start;
  if (history_count < HISTORY_SIZE) {
    start = 0;
  } else {
    start = history_index; /* oldest entry */
  }

  for (int i = 0; i < history_count; i++) {
    int idx = math_modulo(start + i, HISTORY_SIZE);
    char num_buf[8];
    int_to_string(i + 1, num_buf);

    screen_print("  ");
    screen_print(num_buf);
    screen_print(". ");
    screen_print_line(history[idx]);
  }
}

/* ══════════════════════════════════════════════════════════════════ */
/*                     COMMAND HANDLERS                              */
/* ══════════════════════════════════════════════════════════════════ */

/*
 * cmd_help — Prints list of available commands.
 */
static void cmd_help(void) {
  screen_newline();
  screen_print_line("  ╔═════════════════════════════════════════════════════════╗");
  screen_print_line("  ║                 Jarvis-miniOS Commands                  ║");
  screen_print_line("  ╠═════════════════════════════════════════════════════════╣");
  screen_print_line("  ║ SYSTEM:  help, about, history, clear, exit              ║");
  screen_print_line("  ║ MEMORY:  meminfo, memtest <txt>, free <idx>             ║");
  screen_print_line("  ║ PROCESS: run counter, run snake, ps, top, kill <pid>    ║");
  screen_print_line("  ║ FILE:    touch <f>, write <f> <d>, read <f>, rm <f>, ls ║");
  screen_print_line("  ║ FILE:    chmod <f> <perms> (read-only / read-write)     ║");
  screen_print_line("  ║ NETWORK: ping <ip>, send <ip> <msg>                     ║");
  screen_print_line("  ║ UI:      theme <t>, dashboard                           ║");
  screen_print_line("  ║ UTILS:   echo <txt>, calc <a> <op> <b>, trace on/off    ║");
  screen_print_line("  ║ USERS:   login <user>, whoami                           ║");
  screen_print_line("  ║ OTHER:   diagnostics, install <pkg>                     ║");
  screen_print_line("  ╚═════════════════════════════════════════════════════════╝");
  screen_newline();
}

/*
 * cmd_echo — Echoes user text.
 * Pipeline: tokenized args → allocate memory → copy string → print → free
 */
static void cmd_echo(char tokens[][MAX_TOKEN_LEN], int token_count) {
  if (token_count < 2) {
    screen_print_line("  (empty)");
    return;
  }

  /*
   * Reconstruct the text from tokens[1..n]
   * We first compute total length needed, then allocate in virtual RAM.
   */
  int total_len = 0;
  for (int i = 1; i < token_count; i++) {
    total_len += str_length(tokens[i]);
    if (i < token_count - 1) {
      total_len += 1; /* space between words */
    }
  }
  total_len += 1; /* null terminator */

  /* Allocate in virtual RAM (demonstrates the pipeline) */
  char *output = (char *)mem_alloc(total_len);
  if (output == NULL) {
    screen_print_line("ERROR: Memory allocation failed");
    return;
  }

  /* Build the output string manually */
  output[0] = '\0';
  for (int i = 1; i < token_count; i++) {
    str_concat(output, tokens[i]);
    if (i < token_count - 1) {
      /* Append a space */
      int len = str_length(output);
      output[len] = ' ';
      output[len + 1] = '\0';
    }
  }

  /* Print through screen driver */
  screen_print("  ");
  screen_print_line(output);

  /* Free the allocated memory */
  mem_free(output);
}

/*
 * cmd_about — Prints system information using custom functions.
 */
static void cmd_about(void) {
  char used_buf[16], free_buf[16], total_buf[16];

  int_to_string(mem_get_used(), used_buf);
  int_to_string(mem_get_free(), free_buf);
  int_to_string(VIRTUAL_RAM_SIZE, total_buf);

  screen_newline();
  screen_print_line("  ┌─────────────────────────────────────────┐");
  screen_print_line("  │            CodeOS v1.0                  │");
  screen_print_line("  │   Jack OS-Style Mini Operating System   │");
  screen_print_line("  ├─────────────────────────────────────────┤");
  screen_print("  │  Virtual RAM:  ");
  screen_print(total_buf);
  screen_print_line(" bytes              │");
  screen_print("  │  Used:         ");
  screen_print(used_buf);
  screen_print_line(" bytes                   │");
  screen_print("  │  Free:         ");
  screen_print(free_buf);
  screen_print_line(" bytes              │");
  screen_print_line("  │  Libraries:    string, memory, math,    │");
  screen_print_line("  │               screen, keyboard          │");
  screen_print_line("  └─────────────────────────────────────────┘");
  screen_newline();
}

/*
 * cmd_memtest — Allocates memory for user-provided text (persists until freed).
 *
 * Pipeline: keyboard → string → memory → screen
 *   1. Combine tokens[1..n] into a single string  (string.c)
 *   2. Compute length with str_length              (string.c)
 *   3. Allocate memory with mem_alloc              (memory.c)
 *   4. Copy string with str_copy                   (string.c)
 *   5. Track pointer in allocation table
 *   6. Print stored value with screen_print        (screen.c)
 *   Memory is NOT auto-freed — use 'free' command.
 *
 * Usage:
 *   memtest <text>        e.g.  memtest Hello from OS
 */
static void cmd_memtest(char tokens[][MAX_TOKEN_LEN], int token_count) {
  /* ── Validate input ──────────────────────────────────────────── */
  if (token_count < 2) {
    screen_print_line("ERROR: No input provided");
    screen_print_line("  Usage: memtest <text>");
    return;
  }

  /* ── Check allocation slots ──────────────────────────────────── */
  if (alloc_count >= MAX_ALLOCS) {
    screen_print_line("ERROR: Allocation table full (max 16)");
    screen_print_line("  Free some memory first with 'free'");
    return;
  }

  /* ── Combine tokens[1..n] into a temp buffer ─────────────────── */
  char temp[INPUT_BUFFER_SIZE];
  temp[0] = '\0';

  for (int i = 1; i < token_count; i++) {
    str_concat(temp, tokens[i]);
    if (i < token_count - 1) {
      int len = str_length(temp);
      temp[len] = ' ';
      temp[len + 1] = '\0';
    }
  }

  /* ── Compute length & allocate ───────────────────────────────── */
  int alloc_size = str_length(temp) + 1;

  char *block = (char *)mem_alloc(alloc_size);
  if (block == NULL) {
    screen_print_line("ERROR: Memory allocation failed");
    return;
  }

  /* ── Copy into allocated memory ──────────────────────────────── */
  str_copy(block, temp);

  /* ── Track in allocation table ───────────────────────────────── */
  int idx = alloc_count;
  alloc_ptrs[idx] = block;
  alloc_sizes[idx] = alloc_size;
  /* Copy first 31 chars as label */
  int label_len = str_length(temp);
  if (label_len > 31) label_len = 31;
  for (int i = 0; i < label_len; i++) {
    alloc_labels[idx][i] = temp[i];
  }
  alloc_labels[idx][label_len] = '\0';
  alloc_count++;

  /* ── Output ──────────────────────────────────────────────────── */
  screen_print("  [memtest] Allocated ");
  screen_print_int(alloc_size);
  screen_print_line(" bytes");

  screen_print("  [memtest] Stored: ");
  screen_print_line(block);

  char idx_buf[8];
  int_to_string(idx, idx_buf);
  screen_print("  [memtest] Tracked as alloc #");
  screen_print_line(idx_buf);

  screen_print_line("  [memtest] Memory NOT freed (use 'free' command)");
}

/*
 * cmd_free — Frees a tracked allocation.
 *
 * Usage:
 *   free           Frees the last allocation
 *   free <index>   Frees a specific allocation by index
 */
static void cmd_free(char tokens[][MAX_TOKEN_LEN], int token_count) {
  /* ── No allocations to free ──────────────────────────────────── */
  if (alloc_count == 0) {
    screen_print_line("ERROR: No memory to free");
    return;
  }

  int target;

  if (token_count >= 2) {
    /* free <index> — free a specific slot */
    target = str_to_int(tokens[1]);
    if (target < 0 || target >= alloc_count || alloc_ptrs[target] == NULL) {
      screen_print_line("ERROR: Invalid allocation index");
      screen_print("  Valid range: 0 to ");
      screen_print_int(alloc_count - 1);
      screen_newline();
      return;
    }
  } else {
    /* free — find the last non-NULL allocation */
    target = -1;
    for (int i = alloc_count - 1; i >= 0; i--) {
      if (alloc_ptrs[i] != NULL) {
        target = i;
        break;
      }
    }
    if (target == -1) {
      screen_print_line("ERROR: No memory to free");
      return;
    }
  }

  /* ── Free the block ──────────────────────────────────────────── */
  char idx_buf[8];
  int_to_string(target, idx_buf);

  screen_print("  [free] Freeing alloc #");
  screen_print(idx_buf);
  screen_print(" (\"");
  screen_print(alloc_labels[target]);
  screen_print_line("\")");

  screen_print("  [free] Releasing ");
  screen_print_int(alloc_sizes[target]);
  screen_print_line(" bytes");

  mem_free(alloc_ptrs[target]);
  alloc_ptrs[target] = NULL;
  alloc_sizes[target] = 0;
  alloc_labels[target][0] = '\0';

  screen_print_line("  [free] Memory freed successfully");

  /* Compact: if we freed the last slot, shrink count */
  while (alloc_count > 0 && alloc_ptrs[alloc_count - 1] == NULL) {
    alloc_count--;
  }

  screen_print("  [free] Memory used: ");
  screen_print_int(mem_get_used());
  screen_print_line(" bytes");
}

/*
 * cmd_calc — Tests the math library interactively.
 * Usage:
 *   calc <a> <op> <b>     where op is + - * / %
 *   calc abs <n>          absolute value
 *
 * Examples:
 *   calc 6 * 3   → 18
 *   calc 10 / 3  → 3
 *   calc 10 % 3  → 1
 *   calc abs -42 → 42
 */
static void cmd_calc(char tokens[][MAX_TOKEN_LEN], int token_count) {
  /* calc abs <n> */
  if (token_count == 3 && str_compare(tokens[1], "abs") == 0) {
    int n = str_to_int(tokens[2]);
    int result = math_abs(n);
    screen_print("  abs(");
    screen_print_int(n);
    screen_print(") = ");
    screen_print_int(result);
    screen_newline();
    return;
  }

  /* calc <a> <op> <b> */
  if (token_count < 4) {
    screen_print_line("  Usage: calc <a> <op> <b>  (op: + - * / %)");
    screen_print_line("         calc abs <n>");
    return;
  }

  int a = str_to_int(tokens[1]);
  int b = str_to_int(tokens[3]);
  char op = tokens[2][0];
  int result = 0;

  if (op == '+') {
    result = a + b;
  } else if (op == '-') {
    result = a - b;
  } else if (op == '*') {
    result = math_multiply(a, b);
  } else if (op == '/') {
    if (b == 0) {
      screen_print_line("ERROR: Division by zero");
      return;
    }
    result = math_divide(a, b);
  } else if (op == '%') {
    if (b == 0) {
      screen_print_line("ERROR: Modulo by zero");
      return;
    }
    result = math_modulo(a, b);
  } else {
    screen_print("ERROR: Unknown operator '");
    screen_print(tokens[2]);
    screen_print_line("'. Use + - * / %");
    return;
  }

  screen_print("  ");
  screen_print_int(a);
  screen_print(" ");
  screen_print(tokens[2]);
  screen_print(" ");
  screen_print_int(b);
  screen_print(" = ");
  screen_print_int(result);
  screen_newline();
}

/* ══════════════════════════════════════════════════════════════════ */
/*                     BOOT SEQUENCE                                 */
/* ══════════════════════════════════════════════════════════════════ */

static void boot(void) {
  keyboard_init();
  mem_init();
  screen_init();
  ui_init();
  process_init();
  fs_init();
  net_init();

  ui_boot_sequence();
  ui_launcher_menu();
}

static void shutdown(void) {
  screen_newline();
  screen_set_color(COLOR_YELLOW, BG_DEFAULT);
  screen_print_line("  Shutting down Jarvis-miniOS...");
  screen_print_line("  Freeing virtual RAM...");
  mem_shutdown();
  keyboard_shutdown();
  screen_reset_color();
  screen_print_line("  Goodbye!");
  screen_newline();
}

/* ══════════════════════════════════════════════════════════════════ */
/*                     NEW COMMAND HANDLERS                          */
/* ══════════════════════════════════════════════════════════════════ */

static int handle_command(char tokens[][MAX_TOKEN_LEN], int token_count) {
    if (token_count == 0) return 1;

    if (str_compare(tokens[0], "help") == 0) {
        cmd_help();
    } else if (str_compare(tokens[0], "echo") == 0) {
        cmd_echo(tokens, token_count);
    } else if (str_compare(tokens[0], "clear") == 0) {
        screen_clear();
    } else if (str_compare(tokens[0], "about") == 0) {
        cmd_about();
    } else if (str_compare(tokens[0], "meminfo") == 0) {
        screen_newline();
        mem_print_table();
        screen_newline();
    } else if (str_compare(tokens[0], "memtest") == 0) {
        screen_newline();
        cmd_memtest(tokens, token_count);
        screen_newline();
    } else if (str_compare(tokens[0], "free") == 0) {
        screen_newline();
        cmd_free(tokens, token_count);
        screen_newline();
    } else if (str_compare(tokens[0], "calc") == 0) {
        cmd_calc(tokens, token_count);
    } else if (str_compare(tokens[0], "history") == 0) {
        screen_newline();
        history_print();
        screen_newline();
    } else if (str_compare(tokens[0], "run") == 0) {
        if (token_count > 1) {
            if (str_compare(tokens[1], "counter") == 0) {
                int pid = sys_create_process("counter", task_counter);
                sys_print("  [Process] Started counter (PID: ");
                sys_print_int(pid);
                sys_print_line("). Run 'read counter.log' to view its background output!");
            } else if (str_compare(tokens[1], "snake") == 0) {
                task_snake(); /* Foreground task */
            } else if (str_compare(tokens[1], "calc") == 0) {
                sys_print_line("  [Package] Launching calculator...");
            } else {
                sys_print_line("  Unknown task.");
            }
        }
    } else if (str_compare(tokens[0], "ps") == 0) {
        screen_newline();
        process_print_ps();
        screen_newline();
    } else if (str_compare(tokens[0], "top") == 0) {
        process_print_top();
    } else if (str_compare(tokens[0], "kill") == 0) {
        if (token_count > 1) {
            int pid = str_to_int(tokens[1]);
            if (sys_kill_process(pid)) {
                sys_print_line("  Process killed.");
            } else {
                sys_print_line("  Failed to kill process.");
            }
        }
    } else if (str_compare(tokens[0], "touch") == 0) {
        if (token_count > 1) {
            if (sys_create_file(tokens[1]) == 0) sys_print_line("  File created.");
            else sys_print_line("  Failed to create file.");
        }
    } else if (str_compare(tokens[0], "write") == 0) {
        if (token_count > 2) {
            if (fs_write(tokens[1], tokens[2]) == 0) sys_print_line("  Written to file.");
            else sys_print_line("  Write failed.");
        }
    } else if (str_compare(tokens[0], "read") == 0) {
        if (token_count > 1) {
            if (fs_read(tokens[1]) != 0) sys_print_line("  Read failed.");
        }
    } else if (str_compare(tokens[0], "rm") == 0) {
        if (token_count > 1) {
            if (sys_delete_file(tokens[1]) == 0) sys_print_line("  File deleted.");
            else sys_print_line("  Delete failed.");
        }
    } else if (str_compare(tokens[0], "ls") == 0) {
        screen_newline();
        fs_ls();
        screen_newline();
    } else if (str_compare(tokens[0], "chmod") == 0) {
        if (token_count > 2) {
            if (fs_chmod(tokens[1], tokens[2]) == 0) sys_print_line("  Permissions updated.");
            else sys_print_line("  Failed to update permissions.");
        }
    } else if (str_compare(tokens[0], "ping") == 0) {
        if (token_count > 1) {
            screen_newline();
            sys_ping(tokens[1]);
            screen_newline();
        }
    } else if (str_compare(tokens[0], "send") == 0) {
        if (token_count > 2) {
            screen_newline();
            net_send(tokens[1], tokens[2]);
            screen_newline();
        }
    } else if (str_compare(tokens[0], "theme") == 0) {
        if (token_count > 1) {
            if (ui_set_theme(tokens[1])) sys_print_line("  Theme applied.");
            else sys_print_line("  Unknown theme (hacker, retro, ocean).");
        }
    } else if (str_compare(tokens[0], "trace") == 0) {
        if (token_count > 1 && str_compare(tokens[1], "on") == 0) sys_set_trace(1);
        else sys_set_trace(0);
        sys_print_line("  Trace state updated.");
    } else if (str_compare(tokens[0], "diagnostics") == 0) {
        screen_newline();
        sys_print_line("  [DIAGNOSTICS]");
        sys_print_line("  Memory allocator... [OK]");
        sys_print_line("  Process scheduler.. [OK]");
        sys_print_line("  Screen driver...... [OK]");
        screen_newline();
    } else if (str_compare(tokens[0], "login") == 0) {
        if (token_count > 1) {
            str_copy(current_user, tokens[1]);
            sys_print("  Logged in as: ");
            sys_print_line(current_user);
        }
    } else if (str_compare(tokens[0], "whoami") == 0) {
        sys_print_line(current_user);
    } else if (str_compare(tokens[0], "install") == 0) {
        if (token_count > 1) {
            sys_print("  [Package Manager] Installing ");
            sys_print(tokens[1]);
            sys_print_line("... [Done]");
        }
    } else if (str_compare(tokens[0], "dashboard") == 0 || str_compare(tokens[0], "2") == 0) {
        ui_dashboard();
    } else if (str_compare(tokens[0], "1") == 0) {
        screen_clear();
        screen_newline();
        sys_print_line("  [Shell Active]");
    } else if (str_compare(tokens[0], "3") == 0) {
        screen_newline();
        sys_print_line("  [DIAGNOSTICS]");
        sys_print_line("  Memory allocator... [OK]");
        sys_print_line("  Process scheduler.. [OK]");
        sys_print_line("  Screen driver...... [OK]");
        screen_newline();
    } else if (str_compare(tokens[0], "4") == 0) {
        screen_newline();
        fs_ls();
        screen_newline();
    } else if (str_compare(tokens[0], "exit") == 0) {
        return 0;
    } else {
        screen_set_color(COLOR_RED, BG_DEFAULT);
        screen_print("ERROR: Unknown command '");
        screen_print(tokens[0]);
        screen_print_line("'. Type 'help' for available commands.");
        screen_reset_color();
    }
    return 1;
}

/* ══════════════════════════════════════════════════════════════════ */
/*                     SHELL LOOP                                    */
/* ══════════════════════════════════════════════════════════════════ */

static void print_prompt(void) {
    ui_apply_theme();
    screen_print(current_user);
    screen_print("@minios:~$ ");
    fflush(stdout);
}

static void shell_run(void) {
    char input[INPUT_BUFFER_SIZE];
    char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
    int running = 1;
    int input_len = 0;
    input[0] = '\0';

    ui_draw_footer(mem_get_used(), VIRTUAL_RAM_SIZE, process_get_count(), "OK");
    print_prompt();

    while (running) {
        /* Tick background processes */
        process_schedule();

        int c = keyboard_get_char_nonblocking();
        if (c != -1) {
            if (c == '\n' || c == '\r') {
                screen_newline();
                history_add(input);
                
                int token_count = tokenize(input, tokens, MAX_TOKENS);
                running = handle_command(tokens, token_count);
                
                input_len = 0;
                input[0] = '\0';
                
                if (running) {
                    ui_draw_footer(mem_get_used(), VIRTUAL_RAM_SIZE, process_get_count(), "OK");
                    print_prompt();
                }
            } else if (c == 127 || c == '\b') {
                if (input_len > 0) {
                    input_len--;
                    input[input_len] = '\0';
                    screen_print("\b \b");
                }
            } else if (input_len < INPUT_BUFFER_SIZE - 1) {
                input[input_len++] = (char)c;
                input[input_len] = '\0';
                char buf[2] = {(char)c, '\0'};
                screen_print(buf);
            }
        }
        usleep(5000); /* 5ms tick */
    }
}

/* ══════════════════════════════════════════════════════════════════ */
/*                          MAIN                                     */
/* ══════════════════════════════════════════════════════════════════ */

int main(void) {
  boot();
  shell_run();
  shutdown();
  return 0;
}
