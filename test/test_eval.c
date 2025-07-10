// test_calc.c - Full test suite for your calculator
#define _GNU_SOURCE

#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 
#include <string.h> 
#include <stdbool.h>

#include "eval_expr.h"

#define EPSILON 1e-6

static int total_tests = 0;
static int passed_tests = 0;

#define TRY_TEST(expr_str, expected_val) do {  \
total_tests++; \
TRY(eval_expr_err_handler) { \
    double result = eval_expr(expr_str); \
    if (fabs(result - (expected_val)) < EPSILON) { \
        printf("[PASS] %s = %.8f\n", expr_str, result); \
        passed_tests++; \
    } else { \
        printf("[FAIL] %s = %.8f (expected %.8f)\n", expr_str, result, (expected_val)); \
    } \
} EXCEPT() { \
    printf("[FAIL] %s raised an error unexpectedly\n", expr_str); \
} \
} while (0)

#define EXPECT_ERROR(expr_str) do {  \
total_tests++; \
TRY(eval_expr_err_handler) { \
    eval_expr(expr_str); \
    printf("[FAIL] %s did NOT raise error\n", expr_str); \
} EXCEPT() { \
    printf("[PASS] %s correctly raised error\n", expr_str); \
    passed_tests++; \
} \
} while (0) 

int main(void) {
    printf("Starting calculator tests...\n\n");

    // Core arithmetic
    TRY_TEST("1 + 2 * 3", 7);
    TRY_TEST("(1 + 2) * 3", 9);
    TRY_TEST("10 / 2 + 3", 8);

    // Power & associativity
    TRY_TEST("2 ^ 3", 8);
    TRY_TEST("2 ^ 3 ^ 2", 512);
    TRY_TEST("(2 ^ 3) ^ 2", 64);

    // Factorial + chaining
    TRY_TEST("0!", 1);
    TRY_TEST("5!", 120);
    TRY_TEST("3! + 4!", 6 + 24);
    TRY_TEST("2 ^ 3!", pow(2, 6));
    TRY_TEST("(3+1)! / 2", 12);

    // Chained factorials (evaluated as nested !)
    TRY_TEST("3!!", tgamma(tgamma(4) + 1));

    // Implicit multiplication
    TRY_TEST("2pi", 2 * M_PI);
    TRY_TEST("3(2+1)", 9);
    TRY_TEST("2(3)(4)", 24);

    // Constants and functions
    TRY_TEST("pi", M_PI);
    TRY_TEST("e", M_E);
    TRY_TEST("sqrt(4)", 2);
    TRY_TEST("abs(-5)", 5);
    TRY_TEST("log(e)", 1);
    TRY_TEST("round(3.5)", 4);
    TRY_TEST("max(3, 8, 2)", 8);
    TRY_TEST("min(3, 8, 2)", 2);

    // 'ans' keyword tests (global result persistence)
    TRY_TEST("3 + 4", 7);               // sets ans = 7
    TRY_TEST("ans * 2", 14);           // uses ans = 7
    TRY_TEST("ans + 1", 15);           // updates ans = 15
    TRY_TEST("ans ^ 2", 225);          // 15^2 = 225

    // Error tests
    EXPECT_ERROR("5 / 0");
    EXPECT_ERROR("5 + ");
    EXPECT_ERROR("5 + (3 * 2");
    EXPECT_ERROR("log()");
    EXPECT_ERROR("unknown(2)");

    printf("\nTest Summary: %d/%d passed\n", passed_tests, total_tests);
    return (total_tests == passed_tests) ? 0 : 1;

}
