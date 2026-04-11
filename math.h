/*
 * math.h — Custom Math Library for CodeOS
 * =========================================
 * No <math.h> dependency. All operations implemented with
 * basic arithmetic and loops.
 */

#ifndef MATH_H
#define MATH_H

/*
 * math_multiply — Returns a * b using repeated addition.
 */
int math_multiply(int a, int b);

/*
 * math_divide — Returns a / b using repeated subtraction.
 * Returns 0 if b is 0 (division by zero).
 */
int math_divide(int a, int b);

/*
 * math_modulo — Returns a % b.
 * Returns 0 if b is 0.
 */
int math_modulo(int a, int b);

/*
 * math_abs — Returns absolute value of x.
 */
int math_abs(int x);

#endif /* MATH_H */
