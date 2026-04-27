/*
 * keyboard.c — Keyboard Input Driver for JarvisOS
 * ================================================
 * Uses termios and select to enable non-blocking input
 * required for background tasks and mini-games.
 *
 * Arrow keys send a 3-byte ANSI escape sequence:
 *   ESC  [  A  → Up
 *   ESC  [  B  → Down
 *   ESC  [  C  → Right
 *   ESC  [  D  → Left
 * This driver detects those sequences and returns the KEY_* constants
 * defined in keyboard.h so the shell can handle them as single events.
 */

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include "keyboard.h"

static struct termios orig_termios;
static int term_initialized = 0;

/* ── keyboard_init ────────────────────────────────────────────────── */
// Crucial function. Modifies the POSIX `termios` flags to disable `ICANON` and `ECHO`, switching the terminal into raw mode.
void keyboard_init(void) {
    if (!term_initialized) {
        tcgetattr(STDIN_FILENO, &orig_termios);
        struct termios raw = orig_termios;
        /* Disable canonical mode and echo */
        raw.c_lflag &= ~(ICANON | ECHO);
        /* Set read to return after 1 byte with 0 timeout */
        raw.c_cc[VMIN]  = 1;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
        term_initialized = 1;
    }
}

/* ── keyboard_shutdown ────────────────────────────────────────────── */
// Restores original `termios` settings before exit.
void keyboard_shutdown(void) {
    if (term_initialized) {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
        term_initialized = 0;
    }
}

/* ── keyboard_read_line ───────────────────────────────────────────── */
// A blocking input reader used in older phases. Switches back to canonical mode temporarily to use `fgets`, then restores raw mode.
// This function is used to read user input from the keyboard and handle various edge cases, such as empty input, end of file, and invalid input.
int keyboard_read_line(char *buffer, int max_len) {
    if (buffer == NULL || max_len <= 0) return 0;

    /* Temporarily switch back to canonical for fgets */
    if (term_initialized) tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);

    if (fgets(buffer, max_len, stdin) == NULL) {
        buffer[0] = '\0';
        if (term_initialized) {
            struct termios raw = orig_termios;
            raw.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &raw);
        }
        return 0;
    }

    int i = 0;
    while (buffer[i] != '\0') {
        if (buffer[i] == '\n' || buffer[i] == '\r') {
            buffer[i] = '\0';
            break;
        }
        i++;
    }

    /* Restore raw mode */
    if (term_initialized) {
        struct termios raw = orig_termios;
        raw.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }
    return 1;
}

/* ── keyboard_key_pressed ─────────────────────────────────────────── */
// Uses POSIX `select()` to peek at the input buffer to see if a key is waiting, returning immediately.
int keyboard_key_pressed(void) {
    struct timeval tv = {0L, 0L};
    fd_set fds; //file descriptors
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

/*
 * read_byte_nonblocking — tries to read one byte with a very short
 * timeout (used when consuming escape sequences).
 * Returns the byte, or -1 if nothing is available.
 */
static int read_byte_nonblocking(void) {
    struct timeval tv = {0L, 50000L}; /* 50 ms */
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
        unsigned char b;
        if (read(STDIN_FILENO, &b, 1) == 1) return (int)b;
    }
    return -1;
}

/* ── keyboard_get_char_nonblocking ────────────────────────────────── */
/*
 * Non-blocking read.  Detects ANSI escape sequences for arrow keys
 * and returns the KEY_* constant.  Regular characters are returned
 * as-is.  Returns -1 when no key is waiting.
 */
// If `key_pressed` is true, reads the character; else returns -1. 
// The foundation of the multitasking shell.
int keyboard_get_char_nonblocking(void) {
    if (!keyboard_key_pressed()) return -1;

    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return -1;

    /* Escape sequence starts with 0x1B */
    if (c == 0x1B) {
        int b2 = read_byte_nonblocking();
        if (b2 == '[') {
            int b3 = read_byte_nonblocking();
            switch (b3) {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
                case 'H': return KEY_HOME;
                case 'F': return KEY_END;
                case '3': {
                    /* DEL is ESC [ 3 ~ */
                    int b4 = read_byte_nonblocking();
                    if (b4 == '~') return KEY_DEL;
                    return -1;
                }
                default:  return -1;
            }
        }
        /* Plain ESC with nothing after it — ignore */
        return -1;
    }

    return (int)c;
}
