#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "eval.h"

#define EPSILON 1e-9

int failed = 0;
int passed = 0;

void test_ok(const char *expr, double expected) {
    TRY(eval_expr_err_handler) {
        double result = eval_expr(expr);
        if (fabs(result - expected) > EPSILON) {
            printf("❌ FAILED: %s\n    ↳ Expected: %.9f\n    ↳ Got:      %.9f\n", expr, expected, result);
            failed++;
        } else {
            printf("✅ PASSED: %s = %.9f\n", expr, result);
            passed++;
        }
    } EXCEPT() {
        printf("❌ FAILED: %s\n    ↳ Threw an error\n", expr);
        failed++;
    }
}

void test_error(const char *expr) {
    TRY(eval_expr_err_handler) {
        double result = eval_expr(expr);
        printf("❌ FAILED: %s\n    ↳ Expected error but got %.9f\n", expr, result);
        failed++;
    } EXCEPT() {
        printf("✅ PASSED (error): %s\n", expr);
        passed++;
    }
}

int main(void) {
    puts("🧪 Running calculator tests...\n");

    // Basic arithmetic
    test_ok("1+2", 3);
    test_ok("2*3", 6);
    test_ok("4/2", 2);
    test_ok("8-3", 5);
    test_ok("8*5/7 + 9 * 25 +++3", 8.0 * 5 / 7 + 9 * 25 + 3);
    test_ok("-+8-----5*3+1--+-+-9", -+8 - 5 * 3 + 1 + -+-9);
    test_ok("920*78/76*5 + 3 -89*71/2", 920.0 * 78 / 76 * 5 + 3 - 89.0 * 71 / 2);

    // Parentheses
    test_ok("2*(3+4)", 14);
    test_ok("2 * 5 / (4+3) * (23 + (23 * 5 / (6+9)))", 2 * 5.0 / (4 + 3) * (23 + (23 * 5.0 / (6 + 9))));
    test_ok("(((((((5)))))))", 5);

    // Constants
    test_ok("pi", M_PI);
    test_ok("e", M_E);
    test_ok("tau", 2 * M_PI);
    test_ok("phi", 1.618033988749895);
    test_ok("90deg", 90 * 0.017453292519943295);
    test_ok("pi rad", M_PI * 57.29577951308232);

    // Functions
    test_ok("sin(90deg)", 1);
    test_ok("log(e)", 1);
    test_ok("ln(e)", 1);
    test_ok("|-5|", 5);
    test_ok("round(4.4)", 4);
    test_ok("sqrt(4)", 2);
    test_ok("pow(2, 3)", 8);
    test_ok("min(5,2,8)", 2);
    test_ok("max(5,2,8)", 8);
    test_ok("factorial(4)", 24);
    test_ok("gamma(5)", tgamma(5));
    test_ok("mod(9,4)", 1);
    test_ok("cos 0", 1);

    // Operators: ^, !, %
    test_ok("2^3", 8);
    test_ok("2^2^3", 256);
    test_ok("(2^2)^3", 64);
    test_ok("2^3*5", 40);
    test_ok("1/5*2^3*5", 8);
    test_ok("5!", 120);
    test_ok("3!!", 720);
    test_ok("4!*2", 48);
    test_ok("3*4!*2", 48 * 3);
    test_ok("pi!", tgamma(M_PI+1));
    test_ok("7 % 4", 3);
    test_ok("7 % 4 ^ 2", 7); // 7 % (4^2)
    test_ok("9 % 5!", 9); 

    // Implicit multiplication
    test_ok("2pi", 2 * M_PI);
    test_ok("2(3+4)", 14);
    test_ok("2cos(0)", 2 );
    test_ok("(5+6)(2+3)", 55);
    test_ok("(5+6)/2(2+3)", 55.0/ 2);
    test_ok("2/3pi", 2.0 / 3 * M_PI);
    test_ok("2asin(sin(90deg)+tau)rad", 180);

    // Complex nested expressions
    test_ok("((2+3)*4)^2", pow(20, 2));
    test_ok("min(3, max(2,1+1), sqrt(16))", 2);
    test_ok("abs(-5 + min(3,2)^2)", 1);
    test_ok("((2+3)!)^2", pow(120, 2));
    test_ok("2(3 + 4(5 + 6))", 94); // 2*(3+4*11) = 2*(3+44) = 94
    test_ok("abs(round(-4.6)) + max(2,3,4)", 9);

    // Errors
    test_error("5+");
    test_error("sin()");
    test_error("1/0");
    test_error("unknown(5)");

    // Ans
    test_ok("10", 10);          // ans = 10
    test_ok("ans + 5", 15);     // ans = 15
    test_ok("ans^2", 225);      // ans = 225
    test_ok("ans + 4!", 225 + 24); // ans = 249

    printf("\n🎉 Summary: %d passed, %d failed\n", passed, failed);
    return failed > 0 ? 1 : 0;
}