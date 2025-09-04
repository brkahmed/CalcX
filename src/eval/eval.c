#include "eval.h"

#include <ctype.h>
#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define _c_or_eof(c) (c == '\0' ? "end of input" : (char[]){c, '\0'})
#define CHECK_RECURSION_DEPTH(e)                                                                                       \
    do {                                                                                                               \
        if (recursion_depth++ > MAX_RECURSION_DEPTH)                                                                   \
            _error(MaxRecursionDepthError, "Maximum recursion depth of %d exceeded", MAX_RECURSION_DEPTH);             \
        e;                                                                                                             \
        recursion_depth--;                                                                                             \
    } while (0)

typedef const char **ptr;

char eval_error_msg[EVAL_ERROR_MSG_LEN];
EvalErrorType eval_error_type = NoError;
Number eval_last_result       = 0;

static jmp_buf eval_env;
static size_t recursion_depth = 0;

static Number expression(ptr expr);

Number eval(const char *expr) {
    eval_error_msg[0] = '\0';
    eval_error_type   = NoError;
    recursion_depth   = 0;
    if (setjmp(eval_env) == 0) {
        return eval_last_result = expression(&expr);
    } else {
        return NAN;
    }
}

static __attribute__((noreturn)) void _error(EvalErrorType error_type, const char *fmt, ...) {
    eval_error_type = error_type;
    va_list args;
    va_start(args, fmt);
    vsnprintf(eval_error_msg, EVAL_ERROR_MSG_LEN * sizeof(char), fmt, args);
    va_end(args);
    longjmp(eval_env, error_type);
}

static inline void skipspace(ptr curr) {
    while (isspace(**curr)) (*curr)++;
}

static void eatchar(ptr curr, char c) {
    skipspace(curr);
    if (**curr == c)
        (*curr)++;
    else
        _error(SyntaxError, "Expected '%c', got '%s'", c, _c_or_eof(**curr));
}

static Number primary(ptr curr) {
    skipspace(curr);
    if (isdigit(**curr) || (**curr == '.' && isdigit(*(*curr + 1)))) {
        char *end;
        Number result = strtold(*curr, &end);
        *curr         = end;
        return result;
    }
    if (**curr == '(') {
        (*curr)++;
        Number result = expression(curr);
        eatchar(curr, ')');
        return result;
    }
    if (**curr == '|') {
        (*curr)++;
        Number result = expression(curr);
        eatchar(curr, '|');
        return result < 0 ? -result : result;
    }
    // ! TODO
    // ! add variable and function handling
    _error(SyntaxError, "Expected a number or parenthesized expression, got '%s'", _c_or_eof(**curr));
}

static Number factorial(ptr curr) {
    Number result = primary(curr);
    skipspace(curr);
    while (**curr == '!') {
        result = tgammal(result + 1);
        (*curr)++;
        skipspace(curr);
    }
    return result;
}

static Number exponent(ptr curr) {
    Number result = factorial(curr);
    skipspace(curr);
    if (**curr == '^') {
        (*curr)++;
        CHECK_RECURSION_DEPTH(result = powl(result, exponent(curr)));
    }
    return result;
}

static Number unary(ptr curr) {
    skipspace(curr);
    bool is_negative = false;
    while (**curr == '+' || **curr == '-') {
        if (**curr == '-') is_negative = !is_negative;
        (*curr)++;
        skipspace(curr);
    }
    Number result = exponent(curr);
    return is_negative ? -result : result;
}

static Number factor(ptr curr) {
    Number result = unary(curr);
    Number denom;
    while (true) {
        skipspace(curr);
        switch (**curr) {
            case '*':
                (*curr)++;
                result *= unary(curr);
                break;
            case '/':
                (*curr)++;
                if ((denom = unary(curr)) == 0.0)
                    _error(DivisionByZeroError, "Division by zero (%Lf / %Lf)", result, denom);
                result /= denom;
                break;
            case '%':
                (*curr)++;
                if ((denom = unary(curr)) == 0.0)
                    _error(ModuloByZeroError, "Modulo by zero (%Lf %% %Lf)", result, denom);
                result = fmodl(result, denom);
                break;
            case '(': result *= primary(curr); break;
            default:
                if (isalpha(**curr))
                    result *= primary(curr);
                else
                    return result;
        }
    }
}

static Number term(ptr curr) {
    Number result = factor(curr);
    while (true) {
        skipspace(curr);
        switch (**curr) {
            case '+':
                (*curr)++;
                result += factor(curr);
                break;
            case '-':
                (*curr)++;
                result -= factor(curr);
                break;
            default: return result;
        }
    }
}

static Number expression(ptr curr) {
    Number result;
    CHECK_RECURSION_DEPTH(result = term(curr));
    return result;
}
