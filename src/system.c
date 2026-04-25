/*
 * system.c — System Call Layer
 * ==============================
 */

#include "system.h"
#include "screen.h"
#include "keyboard.h"
#include "memory.h"
#include "string.h"

/* Forward declarations for modules we will link against */
extern int process_create(const char *name, void (*task_func)(void));
extern int process_kill(int pid);
extern int fs_touch(const char *name);
extern int fs_rm(const char *name);
extern int net_ping(const char *ip);

static int trace_enabled = 0;

void sys_set_trace(int enabled) {
    trace_enabled = enabled;
}

int sys_get_trace(void) {
    return trace_enabled;
}

/* ── Wrappers for core hardware/libraries ─────────────────────────── */

void sys_print(const char *text) {
    if (trace_enabled) {
        screen_set_color(COLOR_MAGENTA, BG_DEFAULT);
        screen_print("[TRACE] string -> screen: ");
        screen_reset_color();
    }
    screen_print(text);
}

void sys_print_line(const char *text) {
    if (trace_enabled) {
        screen_set_color(COLOR_MAGENTA, BG_DEFAULT);
        screen_print("[TRACE] string -> screen: ");
        screen_reset_color();
    }
    screen_print_line(text);
}

void sys_print_int(int value) {
    screen_print_int(value);
}

int sys_readline(char *buffer, int max_len) {
    int res = keyboard_read_line(buffer, max_len);
    if (res && trace_enabled) {
        screen_set_color(COLOR_MAGENTA, BG_DEFAULT);
        screen_print_line("[TRACE] keyboard -> string");
        screen_reset_color();
    }
    return res;
}

void* sys_alloc(int size) {
    if (trace_enabled) {
        screen_set_color(COLOR_MAGENTA, BG_DEFAULT);
        screen_print_line("[TRACE] -> memory_alloc");
        screen_reset_color();
    }
    return mem_alloc(size);
}

void sys_free(void *ptr) {
    if (trace_enabled) {
        screen_set_color(COLOR_MAGENTA, BG_DEFAULT);
        screen_print_line("[TRACE] -> memory_free");
        screen_reset_color();
    }
    mem_free(ptr);
}

/* ── Wrappers for Higher Level Services ───────────────────────────── */

int sys_create_process(const char *name, void (*task_func)(void)) {
    return process_create(name, task_func);
}

int sys_kill_process(int pid) {
    return process_kill(pid);
}

int sys_create_file(const char *name) {
    return fs_touch(name);
}

int sys_delete_file(const char *name) {
    return fs_rm(name);
}

int sys_ping(const char *ip) {
    return net_ping(ip);
}
