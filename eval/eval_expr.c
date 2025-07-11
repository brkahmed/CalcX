#define _GNU_SOURCE

#include "eval_expr.h"


#include <stdbool.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <math.h>
#include "utility.c"

error_handler eval_expr_err_handler;
double last_result = 0;

static double parse_expr(const char **curr);
static double parse_term(const char **curr);
static double parse_factor(const char **curr);
static double parse_number_or_function(const char **curr);
static double parse_number(const char **curr);
static double parse_const(const char *name);
static double parse_function(const char *name, double *args, size_t args_count);
static size_t parse_function_args(const char **curr, double *args);
static size_t parse_identifier(const char **curr, char *buffer);
static double handle_factorial(const char **curr, double x);
static double handle_expo(const char **curr, double x);

static void skip_space(const char **curr) {
    while (isspace(**curr)) ++(*curr);
}

static void eat_char(const char **curr, char c) {
    skip_space(curr);
    if (**curr == c)
        ++(*curr);
    else
        RAISE(eval_expr_err_handler, SyntaxError, "Expected a '%c' character got '%c'", c, **curr);
}

double eval_expr(const char *expr) {
    double result = parse_expr(&expr);
    if (*expr != '\0')
        RAISE(eval_expr_err_handler, SyntaxError, "Unsupported character; '%c'", *expr);
    last_result = result;
    return result;
}

static double parse_expr(const char **curr) {
    double result = parse_term(curr);
    while (true) {
        skip_space(curr);
        if (**curr == '+') {
            ++(*curr);
            result += parse_term(curr);
        } else if (**curr == '-') {
            ++(*curr);
            result -= parse_term(curr);
        } else
            break;
    }
    return result;
}

static double parse_term(const char **curr) {
    skip_space(curr);
    double result = parse_factor(curr);
    double tmp;
    while (true) {
        skip_space(curr);
        if (**curr == '*') {
            ++(*curr);
            result *= parse_factor(curr);
        } else if (**curr == '(') {
            result *= parse_factor(curr);
        } else if (isalpha(**curr)) {
            result *= parse_number_or_function(curr);
        } else if (**curr == '/') {
            ++(*curr);
            tmp = parse_factor(curr);
            if (tmp == 0) RAISE(eval_expr_err_handler, DivisionByZeroError, "Division by zero");
            result /= tmp;
        } else if (**curr == '%') {
            ++(*curr);
            result = fmod(result, parse_factor(curr));
        } else
            break;
    }
    return result;
}

static double parse_factor(const char **curr) {
    skip_space(curr);
    double result;
    if (**curr == '(') {
        ++(*curr);
        result = parse_expr(curr);
        eat_char(curr, ')');
    } else if (**curr == '|') {
        ++(*curr);
        result = fabs(parse_expr(curr));
        eat_char(curr, '|');
    } else if (**curr == '-') {
        ++(*curr);
        return -parse_factor(curr);
    } else if (**curr == '+') {
        ++(*curr);
        return parse_factor(curr);
    } else {
        result = parse_number_or_function(curr);
    }
    // ? code here handle the result from a expression like (2+3) or number like 5
    skip_space(curr);
    result = handle_factorial(curr, result); // ? return the same number if there is no '!'
    result = handle_expo(curr, result);
    return result;
}

static double handle_expo(const char **curr, double x) {
    while (**curr == '^') {
        ++(*curr);
        x = pow(x, parse_expr(curr));
    }
    return x;
}

static double handle_factorial(const char **curr, double x) {
    while (**curr == '!') {
        ++(*curr);
        x = factorial(x);
        skip_space(curr);
    }
    return x;
}

static double parse_number_or_function(const char **curr) {
    if (isdigit(**curr) || **curr == '.')
        return parse_number(curr);
    else if (isalpha(**curr)) {
        char identifier[MAX_IDENTIFIER_LEN];
        parse_identifier(curr, identifier);

        double result = parse_const(identifier);
        if (!isnan(result)) return result;

        skip_space(curr);
        double args[MAX_FUNCTION_ARGS];
        size_t args_count = parse_function_args(curr, args);
        return parse_function(identifier, args, args_count);
    }
    RAISE(eval_expr_err_handler, SyntaxError, "Expected a number or identifier got '%c'", (**curr == '\0') ? '"' : **curr);
}

static double parse_number(const char **curr) {
    char *end;
    double result = strtod(*curr, &end);
    if (end == *curr)
        RAISE(eval_expr_err_handler, SyntaxError, "Expected a number got '%c'", **curr);
    *curr = end;
    return result;
}


#define AddConst(_name, _value) if (strcmp(_name, name) == 0) return _value;

static double parse_const(const char *name) {
    AddConst("pi", M_PI);
    AddConst("e", M_E);
    AddConst("tau", M_TAU);
    AddConst("phi", M_PHI);
    AddConst("deg", M_DEG);
    AddConst("rad", M_RAD);
    AddConst("ans", last_result);
    return NAN;
}
#undef AddConst


#define AddFunc(_name, _func, _count, ...) \
    if (strcmp(_name, name) == 0) { \
        if (args_count != _count) \
            RAISE(eval_expr_err_handler, IncorrectArgumentCountError, \
                _name " function expect %ld argument(s) but %ld was given ", (unsigned long) _count, args_count); \
        return _func(__VA_ARGS__); \
    }
#define AddOneArgFunc(_name, _func) AddFunc(_name, _func, 1, args[0])
#define AddTwoArgFunc(_name, _func) AddFunc(_name, _func, 2, args[0], args[1])

static double parse_function(const char *name, double *args, size_t args_count) {
    // Trigonometric
    AddOneArgFunc("sin", sin);
    AddOneArgFunc("cos", cos);
    AddOneArgFunc("tan", tan);
    AddOneArgFunc("asin", asin); AddOneArgFunc("arcsin", asin);
    AddOneArgFunc("acos", acos); AddOneArgFunc("arccos", acos);
    AddOneArgFunc("atan", atan); AddOneArgFunc("arctan", atan);
    AddTwoArgFunc("atan2", atan2); AddTwoArgFunc("arctan2", atan2);

    // Hyperbolic
    AddOneArgFunc("sinh", sinh);
    AddOneArgFunc("cosh", cosh);
    AddOneArgFunc("tanh", tanh);
    AddOneArgFunc("asinh", asinh); AddOneArgFunc("arcsinh", asinh);
    AddOneArgFunc("acosh", acosh); AddOneArgFunc("arccosh", acosh);
    AddOneArgFunc("atanh", atanh); AddOneArgFunc("arctanh", atanh);

    // Exp and log
    AddOneArgFunc("exp", exp);
    AddOneArgFunc("exp2", exp2);
    AddOneArgFunc("ln", log); AddOneArgFunc("log", log);
    AddOneArgFunc("log10", log10);
    AddOneArgFunc("log2", log2);

    // Rounding
    AddOneArgFunc("ceil", ceil);
    AddOneArgFunc("floor", floor);
    AddOneArgFunc("trunc", trunc);
    AddOneArgFunc("round", round);
    AddOneArgFunc("rint", rint);

    // Power and root
    AddOneArgFunc("sqrt", sqrt);
    AddOneArgFunc("cbrt", cbrt);
    AddTwoArgFunc("pow", pow);
    AddTwoArgFunc("hypot", hypot);

    // Other
    AddOneArgFunc("abs", fabs);
    AddTwoArgFunc("mod", fmod);

    AddFunc("factorial", factorial, 1, args[0]);
    AddOneArgFunc("gamma", tgamma);
    AddOneArgFunc("erf", erf);
    AddOneArgFunc("erfc", erfc);

    AddFunc("min", min, args_count, args, args_count);
    AddFunc("max", max, args_count, args, args_count);

    RAISE(eval_expr_err_handler, UnsupportedFunctionError, "Unsupported function '%s'", name);
}

#undef AddFunc
#undef AddOneArgFunc
#undef AddTwoArgFunc

static size_t parse_function_args(const char **curr, double *args) {
    skip_space(curr);

    if (**curr != '(') { // ? Handle calling function like sin 0 is equivelent to sin(0)
        args[0] = parse_factor(curr);
        return 1;
    }

    ++(*curr);
    if (**curr == ')') { ++(*curr); return 0; } // ? handle zero args function

    size_t args_count = 0;
    while (args_count < MAX_FUNCTION_ARGS) {
        args[args_count] = parse_expr(curr);
        ++args_count;
        skip_space(curr);
        if (**curr == ')') { ++(*curr); break; }
        eat_char(curr, ',');
    }
    if (args_count >= MAX_FUNCTION_ARGS)
        RAISE(eval_expr_err_handler, MoreThanMaxError, "Too many args, max supported args is %ld", MAX_FUNCTION_ARGS);
    return args_count;
}

static size_t parse_identifier(const char **curr, char *buffer) {
    if (!isalpha(**curr))
        RAISE(eval_expr_err_handler, SyntaxError, "Identifier should start with  alphabetic character not '%c'", **curr);
    const char *start = *curr;
    // ? the - 1 bcz we need to store '\0' at the end
    while (*curr - start < MAX_IDENTIFIER_LEN - 1 && isalnum(**curr)) ++(*curr);
    size_t len = *curr - start;
    if (len >= MAX_IDENTIFIER_LEN - 1)
        RAISE(eval_expr_err_handler, MoreThanMaxError, "Too long Identifier, expected less than %lu characters", len);
    while (start < *curr) {
        *buffer = tolower(*start); // ! for now identifier are case-insensitive
        ++buffer; ++start;
    }
    *buffer = '\0';

    return len;
}