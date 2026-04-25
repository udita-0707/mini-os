/*
 * math.c — Custom Math Library for CodeOS
 * =========================================
 * Implements low-level, algorithmic math functions using bitwise
 * operations and optimized loops. No <math.h> dependencies.
 */

#include "math.h"

/* ── Core Arithmetic (Optimized) ──────────────────────────────────── */

/*
 * math_multiply: Russian Peasant Multiplication (Shift-and-Add)
 * Instead of O(N) repeated addition, this uses O(log N) bit shifts.
 * It halves the multiplier and doubles the multiplicand, adding the
 * multiplicand to the result only when the multiplier is odd.
 */
int math_multiply(int a, int b) {
    int result = 0;
    int negative = 0;

    if (a < 0) { a = -a; negative = !negative; }
    if (b < 0) { b = -b; negative = !negative; }

    while (b > 0) {
        if (b & 1) { /* If multiplier is odd */
            result += a;
        }
        a <<= 1; /* Double a */
        b >>= 1; /* Halve b */
    }

    return negative ? -result : result;
}

/*
 * math_divide: Binary Long Division
 * Achieves O(log N) division by shifting the divisor left to align with
 * the most significant bit of the dividend, then subtracting and shifting right.
 */
int math_divide(int a, int b) {
    if (b == 0) return 0; /* Division by zero safeguard */

    int negative = 0;
    if (a < 0) { a = -a; negative = !negative; }
    if (b < 0) { b = -b; negative = !negative; }

    int quotient = 0;
    
    /* We use an unsigned cast internally to safely handle large shifts */
    unsigned int n = (unsigned int)a;
    unsigned int d = (unsigned int)b;
    unsigned int temp = 1;

    /* Align divisor with dividend */
    while (d <= n) {
        d <<= 1;
        temp <<= 1;
    }

    while (temp > 1) {
        d >>= 1;
        temp >>= 1;
        if (n >= d) {
            n -= d;
            quotient += temp;
        }
    }

    return negative ? -quotient : quotient;
}

/*
 * math_modulo: Uses binary long division logic
 * Identical to division, but we return the remainder.
 */
int math_modulo(int a, int b) {
    if (b == 0) return 0;

    int negative = (a < 0);
    if (a < 0) a = -a;
    if (b < 0) b = -b;

    unsigned int n = (unsigned int)a;
    unsigned int d = (unsigned int)b;

    while (d <= n) {
        d <<= 1;
    }

    while (d > (unsigned int)b) {
        d >>= 1;
        if (n >= d) {
            n -= d;
        }
    }

    int remainder = (int)n;
    return negative ? -remainder : remainder;
}

int math_abs(int x) {
    return (x < 0) ? -x : x;
}

/* ── Boundary / Utility ───────────────────────────────────────────── */

int math_min(int a, int b) {
    return (a < b) ? a : b;
}

int math_max(int a, int b) {
    return (a > b) ? a : b;
}

int math_clamp(int value, int lower, int upper) {
    if (value < lower) return lower;
    if (value > upper) return upper;
    return value;
}

/* ── Bitwise Helpers ──────────────────────────────────────────────── */

int math_is_power_of_two(int n) {
    /* Power of 2 has exactly 1 bit set. (n & (n-1)) clears lowest set bit. */
    if (n <= 0) return 0;
    return (n & (n - 1)) == 0;
}

int math_next_power_of_two(int n) {
    if (n <= 0) return 1;
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

int math_set_bit(int n, int pos) {
    return n | (1 << pos);
}

int math_clear_bit(int n, int pos) {
    return n & ~(1 << pos);
}

int math_toggle_bit(int n, int pos) {
    return n ^ (1 << pos);
}

/* ── Numeric Utilities ────────────────────────────────────────────── */

/* Exponentiation by squaring: O(log exp) */
int math_power(int base, int exp) {
    if (exp < 0) return 0; /* Integer math does not support fractions well */
    int result = 1;
    while (exp > 0) {
        if (exp & 1) result = math_multiply(result, base);
        base = math_multiply(base, base);
        exp >>= 1;
    }
    return result;
}

/* Bitwise integer square root: O(log n) */
int math_integer_sqrt(int n) {
    if (n < 0) return -1; /* Error */
    
    int result = 0;
    int bit = 1 << 30; /* Second-to-top bit */

    /* "bit" starts at the highest power of four <= n */
    while (bit > n) {
        bit >>= 2;
    }

    while (bit != 0) {
        if (n >= result + bit) {
            n -= result + bit;
            result = (result >> 1) + bit;
        } else {
            result >>= 1;
        }
        bit >>= 2;
    }
    return result;
}

/* Euclidean algorithm: O(log(min(a,b))) */
int math_gcd(int a, int b) {
    a = math_abs(a);
    b = math_abs(b);
    while (b != 0) {
        int temp = b;
        b = math_modulo(a, b);
        a = temp;
    }
    return a;
}

int math_lcm(int a, int b) {
    if (a == 0 || b == 0) return 0;
    /* Divide first to prevent intermediate overflow */
    return math_abs(math_multiply(a / math_gcd(a, b), b));
}

/* ── Overflow / Safety Helpers ────────────────────────────────────── */

int math_safe_add(int a, int b, int *overflow_flag) {
    *overflow_flag = 0;
    if ((b > 0) && (a > 2147483647 - b)) *overflow_flag = 1; /* MAX_INT */
    if ((b < 0) && (a < -2147483648 - b)) *overflow_flag = 1; /* MIN_INT */
    return a + b;
}

int math_safe_sub(int a, int b, int *underflow_flag) {
    *underflow_flag = 0;
    if ((b > 0) && (a < -2147483648 + b)) *underflow_flag = 1;
    if ((b < 0) && (a > 2147483647 + b)) *underflow_flag = 1;
    return a - b;
}

/* ── Alignment Helpers ────────────────────────────────────────────── */

int math_align_up(int value, int boundary) {
    if (!math_is_power_of_two(boundary)) return value; /* Requires power of 2 */
    return (value + boundary - 1) & ~(boundary - 1);
}

int math_align_down(int value, int boundary) {
    if (!math_is_power_of_two(boundary)) return value; /* Requires power of 2 */
    return value & ~(boundary - 1);
}
