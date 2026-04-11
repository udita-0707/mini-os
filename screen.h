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

#endif /* SCREEN_H */
