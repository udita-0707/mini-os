/*
 * keyboard.h — Keyboard Input Driver for CodeOS
 * ================================================
 * Abstracts terminal input behind a device-style API.
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

/* ── Special key constants ───────────────────────────────────────────
 * Returned by keyboard_get_char_nonblocking() when an arrow key or
 * other special escape sequence is detected.  All values are chosen
 * to be outside the printable ASCII range (>= 256) so they cannot
 * be confused with real characters.
 */
#define KEY_UP    256
#define KEY_DOWN  257
#define KEY_LEFT  258
#define KEY_RIGHT 259
#define KEY_HOME  260
#define KEY_END   261
#define KEY_DEL   262

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
 * keyboard_key_pressed — Non-blocking check: returns 1 if a byte is
 * waiting in stdin, 0 otherwise.
 */
int keyboard_key_pressed(void);

/*
 * keyboard_get_char_nonblocking — Non-blocking read.
 * Returns the next character code, one of the KEY_* constants above
 * for special keys, or -1 if no key is available.
 */
int keyboard_get_char_nonblocking(void);

#endif /* KEYBOARD_H */
