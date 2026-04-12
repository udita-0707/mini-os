/*
 * string.h — Custom String Library for CodeOS
 * =============================================
 * Replaces <string.h> with manually implemented string operations.
 * All functions operate on raw char arrays — no standard library calls.
 */

#ifndef STRING_H
#define STRING_H

/* Maximum number of tokens from a single input line */
#define MAX_TOKENS 32

/* Maximum length of a single token */
#define MAX_TOKEN_LEN 128

/*
 * str_length — Returns the number of characters in a null-terminated string.
 * Does NOT count the null terminator.
 */
int str_length(const char *s);

/*
 * str_copy — Copies src into dest (including null terminator).
 * Caller must ensure dest has enough space.
 */
void str_copy(char *dest, const char *src);

/*
 * str_compare — Compares two strings character by character.
 * Returns 0 if equal, negative if a < b, positive if a > b.
 */
int str_compare(const char *a, const char *b);

/*
 * str_starts_with — Checks if string s starts with prefix.
 * Returns 1 if true, 0 if false.
 */
int str_starts_with(const char *s, const char *prefix);

/*
 * tokenize — Splits input string by spaces into an array of tokens.
 * tokens[i] will point to token_storage[i] (caller provides both).
 * Returns the number of tokens found.
 */
int tokenize(const char *input, char tokens[][MAX_TOKEN_LEN], int max_tokens);

/*
 * int_to_string — Converts an integer to its string representation.
 * Handles negative numbers. Caller must provide buffer with enough space.
 */
void int_to_string(int num, char *buffer);

/*
 * str_to_int — Converts a numeric string to an integer.
 * Handles optional leading '-' for negatives.
 */
int str_to_int(const char *s);

/*
 * str_concat — Appends src to the end of dest.
 * Caller must ensure dest has enough space.
 */
void str_concat(char *dest, const char *src);

#endif /* STRING_H */
