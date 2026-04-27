/*
 * string.c — Custom String Library for CodeOS
 * =============================================
 * Manual implementations of all string operations.
 * Utility library built from scratch to avoid standard library dependencies.
 * No <string.h> used anywhere.
 */

#include "string.h"

/* ── str_length ───────────────────────────────────────────────────── */
int str_length(const char *s) {
    int len = 0;
    while (s[len] != '\0') {
        len++;
    }
    return len;
}

/* ── str_copy ─────────────────────────────────────────────────────── */
void str_copy(char *dest, const char *src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

/* ── str_compare ──────────────────────────────────────────────────── */
int str_compare(const char *a, const char *b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return a[i] - b[i];
        }
        i++;
    }
    return a[i] - b[i];
}

/* ── str_starts_with ──────────────────────────────────────────────── */
int str_starts_with(const char *s, const char *prefix) {
    int i = 0;
    while (prefix[i] != '\0') {
        if (s[i] == '\0' || s[i] != prefix[i]) {
            return 0;
        }
        i++;
    }
    return 1;
}

/* ── str_concat ───────────────────────────────────────────────────── */
void str_concat(char *dest, const char *src) {
    int dest_len = str_length(dest);
    int i = 0;
    while (src[i] != '\0') {
        dest[dest_len + i] = src[i];
        i++;
    }
    dest[dest_len + i] = '\0';
}

/* ── tokenize ─────────────────────────────────────────────────────── */
/*
 * Splits input by whitespace. Respects quotes to group words.
 * Fills tokens[0..n-1] with null-terminated strings.
 * Returns the number of tokens found.
 */
int tokenize(const char *input, char tokens[][MAX_TOKEN_LEN], int max_tokens) {
    int token_count = 0;
    int i = 0;

    while (input[i] != '\0' && token_count < max_tokens) {
        /* Skip leading whitespace */
        while (input[i] == ' ' || input[i] == '\t' || input[i] == '\n' || input[i] == '\r') {
            i++;
        }

        if (input[i] == '\0') {
            break;
        }

        int j = 0;
        int in_single_quotes = 0;
        int in_double_quotes = 0;

        /* Read a token */
        while (input[i] != '\0' && j < MAX_TOKEN_LEN - 1) {
            /* Toggle quote state and skip the quote character itself */
            if (input[i] == '\'' && !in_double_quotes) {
                in_single_quotes = !in_single_quotes;
                i++;
                continue;
            } else if (input[i] == '"' && !in_single_quotes) {
                in_double_quotes = !in_double_quotes;
                i++;
                continue;
            }

            /* Stop at whitespace if not inside quotes */
            if (!in_single_quotes && !in_double_quotes &&
                (input[i] == ' ' || input[i] == '\t' || input[i] == '\n' || input[i] == '\r')) {
                break;
            }

            tokens[token_count][j] = input[i];
            i++;
            j++;
        }
        tokens[token_count][j] = '\0';
        token_count++;
    }

    return token_count;
}

/* ── int_to_string ────────────────────────────────────────────────── */
/*
 * Converts integer to string. Handles negatives and zero.
 * buffer must be large enough (at least 12 chars for 32-bit int).
 */
void int_to_string(int num, char *buffer) {
    int i = 0;
    int is_negative = 0;

    /* Handle zero */
    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    /* Handle negative */
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    /* Extract digits in reverse order */
    while (num > 0) {
        buffer[i] = '0' + (num % 10);  /* manual modulo replaced below */
        num = num / 10;
        i++;
    }

    if (is_negative) {
        buffer[i] = '-';
        i++;
    }

    buffer[i] = '\0';

    /* Reverse the string in place */
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
}

/* ── str_to_int ───────────────────────────────────────────────────── */
int str_to_int(const char *s) {
    int result = 0;
    int i = 0;
    int sign = 1;

    if (s[0] == '-') {
        sign = -1;
        i = 1;
    }

    while (s[i] >= '0' && s[i] <= '9') {
        result = result * 10 + (s[i] - '0');
        i++;
    }

    return result * sign;
}
