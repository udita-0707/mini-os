/*
 * keyboard.c — Keyboard Input Driver for CodeOS
 * ================================================
 * Uses termios and select to enable non-blocking input
 * required for background tasks and mini-games.
 */

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include "keyboard.h"

static struct termios orig_termios;
static int term_initialized = 0;

/* ── keyboard_init ────────────────────────────────────────────────── */
void keyboard_init(void) {
    if (!term_initialized) {
        tcgetattr(STDIN_FILENO, &orig_termios);
        struct termios raw = orig_termios;
        /* Disable canonical mode and echo */
        raw.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
        term_initialized = 1;
    }
}

/* ── keyboard_shutdown ────────────────────────────────────────────── */
void keyboard_shutdown(void) {
    if (term_initialized) {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
        term_initialized = 0;
    }
}

/* ── keyboard_read_line ───────────────────────────────────────────── */
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
int keyboard_key_pressed(void) {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

/* ── keyboard_get_char_nonblocking ────────────────────────────────── */
int keyboard_get_char_nonblocking(void) {
    if (keyboard_key_pressed()) {
        char c;
        if (read(STDIN_FILENO, &c, 1) == 1) {
            return c;
        }
    }
    return -1;
}
