/*
 * keyboard.c — Keyboard Input Driver for CodeOS
 * ================================================
 * Reads user input via stdio's fgets, hidden behind a device API.
 * Upper layers never call fgets directly.
 */

#include <stdio.h>
#include "keyboard.h"

/* ── keyboard_read_line ───────────────────────────────────────────── */
int keyboard_read_line(char *buffer, int max_len) {
    if (buffer == NULL || max_len <= 0) {
        return 0;
    }

    /* Read a line from stdin */
    if (fgets(buffer, max_len, stdin) == NULL) {
        buffer[0] = '\0';
        return 0;  /* EOF or error */
    }

    /* Strip trailing newline manually (no strchr) */
    int i = 0;
    while (buffer[i] != '\0') {
        if (buffer[i] == '\n' || buffer[i] == '\r') {
            buffer[i] = '\0';
            break;
        }
        i++;
    }

    return 1;
}

/* ── keyboard_key_pressed ─────────────────────────────────────────── */
int keyboard_key_pressed(void) {
    /*
     * In a real OS this would check the keyboard controller.
     * In our simulation we use a blocking read model,
     * so we always report "key available".
     */
    return 1;
}
