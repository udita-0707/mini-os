/*
 * keyboard.h — Keyboard Input Driver for CodeOS
 * ================================================
 * Abstracts terminal input behind a device-style API.
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

/* Initialize terminal for raw/non-blocking input */
void keyboard_init(void);

/* Restore original terminal settings */
void keyboard_shutdown(void);

/*
 * keyboard_read_line — Reads a line of input from the user.
 * Stores at most max_len-1 characters into buffer, null-terminated.
 * Strips trailing newline if present.
 * Returns 1 on success, 0 on EOF/failure.
 */
int keyboard_read_line(char *buffer, int max_len);

/*
 * keyboard_key_pressed — Simulates checking if a key is available.
 * In this simulation, always returns 1 (blocking read model).
 */
int keyboard_key_pressed(void);

/* Non-blocking read single character, returns -1 if none */
int keyboard_get_char_nonblocking(void);

#endif /* KEYBOARD_H */
