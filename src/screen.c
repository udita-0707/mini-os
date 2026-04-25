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

/* ── UI & ANSI Extensions ─────────────────────────────────────────── */

void screen_set_color(int fg, int bg) {
    if (bg == BG_DEFAULT && fg == COLOR_DEFAULT) {
        screen_reset_color();
    } else if (bg == BG_DEFAULT) {
        printf("\033[%dm", fg);
    } else if (fg == COLOR_DEFAULT) {
        printf("\033[%dm", bg);
    } else {
        printf("\033[%d;%dm", fg, bg);
    }
    fflush(stdout);
}

void screen_reset_color(void) {
    printf("\033[0m");
    fflush(stdout);
}

void screen_set_cursor(int row, int col) {
    printf("\033[%d;%dH", row, col);
    fflush(stdout);
}

void screen_save_cursor(void) {
    printf("\033[s");
    fflush(stdout);
}

void screen_restore_cursor(void) {
    printf("\033[u");
    fflush(stdout);
}

void screen_hide_cursor(void) {
    printf("\033[?25l");
    fflush(stdout);
}

void screen_show_cursor(void) {
    printf("\033[?25h");
    fflush(stdout);
}
