/*
 * screen.h — Screen Output Driver for CodeOS
 * =============================================
 * Abstracts terminal output behind a device-style API.
 * Uses <stdio.h> internally but hides it from upper layers.
 */

#ifndef SCREEN_H
#define SCREEN_H

/*
 * screen_init — Initializes cursor tracking.
 */
void screen_init(void);

/*
 * screen_print — Prints text WITHOUT a newline.
 */
void screen_print(const char *text);

/*
 * screen_print_line — Prints text followed by a newline.
 */
void screen_print_line(const char *text);

/*
 * screen_print_int — Prints an integer value.
 * Uses custom int_to_string internally.
 */
void screen_print_int(int value);

/*
 * screen_clear — Clears the terminal screen using ANSI escapes.
 */
void screen_clear(void);

/*
 * screen_newline — Prints a blank line.
 */
void screen_newline(void);

/* ── UI & ANSI Extensions ─────────────────────────────────────────── */

#define COLOR_DEFAULT 0
#define COLOR_BLACK   30
#define COLOR_RED     31
#define COLOR_GREEN   32
#define COLOR_YELLOW  33
#define COLOR_BLUE    34
#define COLOR_MAGENTA 35
#define COLOR_CYAN    36
#define COLOR_WHITE   37

#define BG_DEFAULT    0
#define BG_BLACK      40
#define BG_RED        41
#define BG_GREEN      42
#define BG_YELLOW     43
#define BG_BLUE       44
#define BG_MAGENTA    45
#define BG_CYAN       46
#define BG_WHITE      47

/* Set text and background colors */
void screen_set_color(int fg, int bg);

/* Reset colors to terminal default */
void screen_reset_color(void);

/* Move cursor to specific row and col (1-indexed) */
void screen_set_cursor(int row, int col);

/* Save and restore cursor position */
void screen_save_cursor(void);
void screen_restore_cursor(void);

/* Hide and show cursor */
void screen_hide_cursor(void);
void screen_show_cursor(void);

#endif /* SCREEN_H */
