#include <stdio.h>
#include <stdlib.h>
#include "math.h"

int tests_passed = 0;
int tests_failed = 0;

void assert_eq(int expected, int actual, const char *test_name) {
    if (expected == actual) {
        tests_passed++;
    } else {
        printf("[FAIL] %s: Expected %d, got %d\n", test_name, expected, actual);
        tests_failed++;
    }
}

int main() {
    printf("Running math library tests...\n");

    /* Core Arithmetic */
    assert_eq(20, math_multiply(4, 5), "multiply pos");
    assert_eq(-20, math_multiply(-4, 5), "multiply neg");
    assert_eq(20, math_multiply(-4, -5), "multiply double neg");
    assert_eq(0, math_multiply(0, 5), "multiply zero");

    assert_eq(5, math_divide(20, 4), "divide pos");
    assert_eq(-5, math_divide(-20, 4), "divide neg");
    assert_eq(5, math_divide(-20, -4), "divide double neg");
    assert_eq(0, math_divide(5, 0), "divide by zero");
    assert_eq(3, math_divide(10, 3), "divide remainder truncates");

    assert_eq(2, math_modulo(17, 5), "modulo pos");
    assert_eq(-2, math_modulo(-17, 5), "modulo neg dividend");
    assert_eq(0, math_modulo(10, 5), "modulo exact");
    assert_eq(0, math_modulo(5, 0), "modulo by zero");

    assert_eq(10, math_abs(10), "abs pos");
    assert_eq(10, math_abs(-10), "abs neg");

    /* Utility */
    assert_eq(3, math_min(3, 8), "min");
    assert_eq(8, math_max(3, 8), "max");
    assert_eq(5, math_clamp(2, 5, 10), "clamp low");
    assert_eq(8, math_clamp(8, 5, 10), "clamp mid");
    assert_eq(10, math_clamp(15, 5, 10), "clamp high");

    /* Bitwise */
    assert_eq(1, math_is_power_of_two(16), "is_power_of_two true");
    assert_eq(0, math_is_power_of_two(18), "is_power_of_two false");
    assert_eq(0, math_is_power_of_two(0), "is_power_of_two zero");
    assert_eq(16, math_next_power_of_two(15), "next_power_of_two 15->16");
    assert_eq(16, math_next_power_of_two(16), "next_power_of_two 16->16");
    assert_eq(5, math_set_bit(4, 0), "set_bit"); /* 4 (100) -> 5 (101) */
    assert_eq(4, math_clear_bit(5, 0), "clear_bit"); /* 5 (101) -> 4 (100) */
    assert_eq(7, math_toggle_bit(5, 1), "toggle_bit"); /* 5 (101) -> 7 (111) */

    /* Numeric Utilities */
    assert_eq(32, math_power(2, 5), "power");
    assert_eq(1, math_power(5, 0), "power zero");
    assert_eq(4, math_integer_sqrt(16), "sqrt exact");
    assert_eq(4, math_integer_sqrt(24), "sqrt floor");
    assert_eq(14, math_gcd(42, 56), "gcd");
    assert_eq(168, math_lcm(42, 56), "lcm");

    /* Alignment */
    assert_eq(16, math_align_up(15, 8), "align_up 15->16");
    assert_eq(16, math_align_up(16, 8), "align_up 16->16");
    assert_eq(8, math_align_down(15, 8), "align_down 15->8");

    /* Safe math */
    int flag;
    assert_eq(10, math_safe_add(5, 5, &flag), "safe_add normal");
    assert_eq(0, flag, "safe_add normal flag");
    math_safe_add(2147483647, 1, &flag);
    assert_eq(1, flag, "safe_add overflow flag");

    assert_eq(5, math_safe_sub(10, 5, &flag), "safe_sub normal");
    assert_eq(0, flag, "safe_sub normal flag");
    math_safe_sub(-2147483648, 1, &flag);
    assert_eq(1, flag, "safe_sub underflow flag");

    printf("\nTest Results: %d passed, %d failed.\n", tests_passed, tests_failed);
    
    if (tests_failed > 0) return 1;
    return 0;
}
