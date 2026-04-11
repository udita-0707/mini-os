/*
 * math.c — Custom Math Library for CodeOS
 * =========================================
 * Implements multiply, divide, modulo, abs
 * entirely with loops — no <math.h>.
 */

#include "math.h"

/* ── math_multiply ────────────────────────────────────────────────── */
/*
 * Handles negative operands by tracking sign separately.
 * Uses repeated addition for simplicity.
 * (For large numbers this is slow, but correct for a simulation.)
 */
int math_multiply(int a, int b) {
    int result = 0;
    int negative = 0;

    /* Handle signs */
    if (a < 0) { a = -a; negative = !negative; }
    if (b < 0) { b = -b; negative = !negative; }

    /* Optimisation: iterate over the smaller operand */
    if (a < b) {
        int temp = a; a = b; b = temp;
    }

    for (int i = 0; i < b; i++) {
        result += a;
    }

    return negative ? -result : result;
}

/* ── math_divide ──────────────────────────────────────────────────── */
int math_divide(int a, int b) {
    if (b == 0) return 0;  /* Guard against division by zero */

    int negative = 0;
    if (a < 0) { a = -a; negative = !negative; }
    if (b < 0) { b = -b; negative = !negative; }

    int quotient = 0;
    while (a >= b) {
        a -= b;
        quotient++;
    }

    return negative ? -quotient : quotient;
}

/* ── math_modulo ──────────────────────────────────────────────────── */
int math_modulo(int a, int b) {
    if (b == 0) return 0;

    int negative = (a < 0);
    if (a < 0) a = -a;
    if (b < 0) b = -b;

    while (a >= b) {
        a -= b;
    }

    return negative ? -a : a;
}

/* ── math_abs ─────────────────────────────────────────────────────── */
int math_abs(int x) {
    return (x < 0) ? -x : x;
}
