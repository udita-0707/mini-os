/*
 * process.c — Process Management & Scheduler
 * ============================================
 */

#include "process.h"
#include "screen.h"
#include "string.h"
#include "system.h"
#include "keyboard.h"
#include "fs.h"
#include <unistd.h>

static Process process_table[MAX_PROCESSES];
static int next_pid = 1;
static int current_process_idx = 0;

// Clears the process table.
void process_init(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].active = 0;
    }
    /* Create shell process (always pid 1) */
    process_create("shell", NULL);
}

// Registers a new function pointer in the scheduler table, assigns a PID, and marks it active.
int process_create(const char *name, void (*task_func)(void)) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (!process_table[i].active) {
            process_table[i].pid = next_pid++;
            str_copy(process_table[i].name, name);
            process_table[i].active = 1;
            process_table[i].priority = 1;
            process_table[i].task_func = task_func;
            return process_table[i].pid;
        }
    }
    return -1;
}

// Marks a PID as inactive, effectively removing it from the execution loop.
int process_kill(int pid) {
    if (pid == 1) return 0; /* Cannot kill shell */
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].active && process_table[i].pid == pid) {
            process_table[i].active = 0;
            return 1;
        }
    }
    return 0;
}

// Iterates the process table by 1 step. If the current index is active, it calls the `task_func()`. 
// Operates on a round-robin cycle.
void process_schedule(void) {
    /* Simple round-robin: run the next active process that has a task_func */
    int start_idx = current_process_idx;
    do {
        current_process_idx = (current_process_idx + 1) % MAX_PROCESSES;
        if (process_table[current_process_idx].active && process_table[current_process_idx].task_func != NULL) {
            /* Execute one tick of the task */
            process_table[current_process_idx].task_func();
            break; /* We only run one task per scheduler tick */
        }
    } while (current_process_idx != start_idx);
}

// Returns the number of running processes for the UI dashboard.
int process_get_count(void) {
    int count = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].active) count++;
    }
    return count;
}

// Prints a formatted table of all active processes.
void process_print_ps(void) {
    sys_print_line("  PID  NAME      STATUS");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].active) {
            screen_print("  ");
            char pid_str[8];
            int_to_string(process_table[i].pid, pid_str);
            screen_print(pid_str);
            
            /* Pad pid */
            for (int p = str_length(pid_str); p < 4; p++) screen_print(" ");
            
            screen_print(" ");
            screen_print(process_table[i].name);
            
            /* Pad name */
            for (int p = str_length(process_table[i].name); p < 10; p++) screen_print(" ");
            
            if (process_table[i].pid == 1) {
                screen_print_line("active");
            } else {
                screen_print_line("running");
            }
        }
    }
}

// A live view simulation that continuously updates the process list until a key is pressed. Demonstrates the scheduler in action.
void process_print_top(void) {
    /* Live view simulation. We just clear screen and print ps once for the demo, 
       or we could loop until key pressed. Let's do a short loop. */
    screen_clear();
    for (int ticks = 0; ticks < 10; ticks++) {
        screen_set_cursor(1, 1);
        screen_set_color(COLOR_CYAN, BG_DEFAULT);
        sys_print_line("  JarvisOS Top Utility (Press ANY KEY to exit)");
        screen_reset_color();
        sys_print_line("  ──────────────────────────────────────────");
        process_print_ps();
        
        usleep(500000); /* 500ms */
        if (keyboard_key_pressed()) {
            keyboard_get_char_nonblocking(); /* consume */
            break;
        }
    }
    screen_clear();
}

/* ── Built-in Tasks ───────────────────────────────────────────────── */

static int counter_val = 0;

//  The logic for the `run counter` background task.
//  Uses the VFS `fs_write` to log its state.
void task_counter(void) {
    counter_val++;
    if (counter_val % 100 == 0) {
        if (fs_touch("counter.log") == -2) return; /* Disk full, ignore */
        
        char buf[64] = "Background process alive. Tick: ";
        char num[16];
        int_to_string(counter_val / 100, num);
        
        /* Append number manually */
        int i = 0; while(buf[i] != '\0') i++;
        int j = 0; while(num[j] != '\0') { buf[i++] = num[j++]; }
        buf[i] = '\0';
        
        fs_write("counter.log", buf);
    }
}

void task_snake(void) {
    /* Blocking mini-game */
    screen_clear();
    screen_set_color(COLOR_GREEN, BG_DEFAULT);
    screen_set_cursor(2, 20);
    sys_print_line("SNAKE MINI-GAME: EAT THE 'X'");
    screen_set_cursor(4, 20);
    sys_print_line("Mission: Use W/A/S/D to move 'O' and eat 'X' to increase score.");
    screen_set_cursor(5, 20);
    sys_print_line("Press 'Q' to quit and return to shell.");
    
    int x = 20, y = 14;
    int score = 0;
    
    unsigned int pseudo_seed = 12345;
    int food_x = 10 + (pseudo_seed % 40);
    int food_y = 10 + (pseudo_seed % 10);
    
    int run = 1;
    while(run) {
        /* Draw score */
        screen_set_cursor(7, 20);
        screen_print("Score: ");
        sys_print_int(score);
        screen_print("    ");
        
        /* Draw food */
        screen_set_cursor(food_y, food_x);
        screen_set_color(COLOR_YELLOW, BG_DEFAULT);
        screen_print("X");
        screen_set_color(COLOR_GREEN, BG_DEFAULT);
        
        /* Draw player */
        screen_set_cursor(y, x);
        screen_print("O");
        
        /* Check collision */
        if (x == food_x && y == food_y) {
            score += 10;
            pseudo_seed = (pseudo_seed * 1103515245 + 12345) & 0x7fffffff;
            food_x = 10 + (pseudo_seed % 40);
            food_y = 10 + (pseudo_seed % 10);
        }
        
        usleep(100000);
        int c = keyboard_get_char_nonblocking();
        if (c != -1) {
            screen_set_cursor(y, x);
            screen_print(" "); /* clear old */
            if (c == 'w') y--;
            if (c == 's') y++;
            if (c == 'a') x--;
            if (c == 'd') x++;
            if (c == 'q') run = 0;
            
            /* Boundaries */
            if (x < 1) x = 1; if (x > 80) x = 80;
            if (y < 8) y = 8; if (y > 24) y = 24;
        }
    }
    
    screen_reset_color();
    screen_clear();
}
