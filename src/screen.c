/*
 * screen.c — Screen Output Driver for CodeOS
 * =============================================
 * Wraps printf behind a device-style API.
 * Upper layers never call printf directly — they go through screen.c.
 */

#include <stdio.h>
#include "screen.h"
#include "string.h"

/* Cursor tracking (row, col) — simulates a text-mode display */
static int cursor_row = 0;
static int cursor_col = 0;

/* ── screen_init ──────────────────────────────────────────────────── */
void screen_init(void) {
    cursor_row = 0;
    cursor_col = 0;
}

/* ── screen_print ─────────────────────────────────────────────────── */
void screen_print(const char *text) {
    /* Print character by character, tracking cursor position */
    int i = 0;
    while (text[i] != '\0') {
        putchar(text[i]);
        if (text[i] == '\n') {
            cursor_row++;
            cursor_col = 0;
        } else {
            cursor_col++;
        }
        i++;
    }
    fflush(stdout);
}

/* ── screen_print_line ────────────────────────────────────────────── */
void screen_print_line(const char *text) {
    screen_print(text);
    putchar('\n');
    cursor_row++;
    cursor_col = 0;
    fflush(stdout);
}

/* ── screen_print_int ─────────────────────────────────────────────── */
void screen_print_int(int value) {
    char buffer[16];
    int_to_string(value, buffer);
    screen_print(buffer);
}

/* ── screen_clear ─────────────────────────────────────────────────── */
void screen_clear(void) {
    /* ANSI escape: clear screen + move cursor to top-left */
    printf("\033[2J\033[H");
    fflush(stdout);
    cursor_row = 0;
    cursor_col = 0;
}

/* ── screen_newline ───────────────────────────────────────────────── */
void screen_newline(void) {
    putchar('\n');
    cursor_row++;
    cursor_col = 0;
    fflush(stdout);
}
