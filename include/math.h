/*
 * math.h — Custom Math Library for CodeOS
 * =========================================
 * No <math.h> dependency. All operations implemented with
 * optimized algorithmic approaches (e.g. bitwise shifts)
 * specifically designed for systems programming.
 */

#ifndef MATH_H
#define MATH_H

/* ── Core Arithmetic (Optimized) ──────────────────────────────────── */

/*
 * math_multiply — Returns a * b.
 * Algorithm: Russian Peasant Multiplication (Shift-and-Add).
 * Complexity: O(log N). Superior to naive repeated addition.
 */
int math_multiply(int a, int b);

/*
 * math_divide — Returns a / b.
 * Algorithm: Binary Long Division.
 * Complexity: O(log N). Superior to naive repeated subtraction.
 * Returns 0 if b is 0 (division by zero).
 */
int math_divide(int a, int b);

/*
 * math_modulo — Returns a % b.
 * Algorithm: Relies on Binary Long Division logic.
 * Complexity: O(log N).
 * Returns 0 if b is 0.
 */
int math_modulo(int a, int b);

/*
 * math_abs — Returns absolute value of x.
 * Algorithm: Simple branching/ternary.
 */
int math_abs(int x);

/* ── Boundary / Utility ───────────────────────────────────────────── */

/* Returns the smaller of a and b */
int math_min(int a, int b);

/* Returns the larger of a and b */
int math_max(int a, int b);

/* Restricts 'value' to be between 'lower' and 'upper' bounds */
int math_clamp(int value, int lower, int upper);

/* ── Bitwise Helpers ──────────────────────────────────────────────── */

/* Returns 1 if n is a power of 2, 0 otherwise */
int math_is_power_of_two(int n);

/* Returns the next power of 2 greater than or equal to n */
int math_next_power_of_two(int n);

/* Sets the bit at 'pos' (0-indexed) in 'n' to 1 */
int math_set_bit(int n, int pos);

/* Clears the bit at 'pos' in 'n' to 0 */
int math_clear_bit(int n, int pos);

/* Toggles the bit at 'pos' in 'n' */
int math_toggle_bit(int n, int pos);

/* ── Numeric Utilities ────────────────────────────────────────────── */

/*
 * math_power — Returns base^exp.
 * Algorithm: Exponentiation by squaring.
 * Complexity: O(log exp).
 */
int math_power(int base, int exp);

/*
 * math_integer_sqrt — Returns the integer square root of n (floor(sqrt(n))).
 * Algorithm: Bitwise binary search / digit-by-digit algorithm.
 * Complexity: O(log n).
 */
int math_integer_sqrt(int n);

/*
 * math_gcd — Returns the Greatest Common Divisor of a and b.
 * Algorithm: Euclidean algorithm.
 * Complexity: O(log(min(a,b))).
 */
int math_gcd(int a, int b);

/*
 * math_lcm — Returns the Least Common Multiple of a and b.
 * Algorithm: (a * b) / gcd(a,b).
 */
int math_lcm(int a, int b);

/* ── Overflow / Safety Helpers ────────────────────────────────────── */

/*
 * Adds a and b. If an overflow occurs, sets *overflow_flag to 1.
 * Otherwise *overflow_flag is set to 0. Returns the wrapped result.
 */
int math_safe_add(int a, int b, int *overflow_flag);

/*
 * Subtracts b from a. If an underflow occurs, sets *underflow_flag to 1.
 * Otherwise *underflow_flag is set to 0. Returns the wrapped result.
 */
int math_safe_sub(int a, int b, int *underflow_flag);

/* ── Alignment Helpers ────────────────────────────────────────────── */

/*
 * math_align_up — Rounds 'value' up to the nearest multiple of 'boundary'.
 * 'boundary' must be a power of two. Essential for memory allocators.
 */
int math_align_up(int value, int boundary);

/*
 * math_align_down — Rounds 'value' down to the nearest multiple of 'boundary'.
 * 'boundary' must be a power of two.
 */
int math_align_down(int value, int boundary);

#endif /* MATH_H */
