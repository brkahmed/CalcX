#include "eval.h"

#include <ctype.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <xmalloc.h>

#include "functions.h"
#include "table.h"
#include "types.h"

#define _c_or_eof(c) (c == '\0' ? "end of input" : (char[]){c, '\0'})
#define CHECK_RECURSION_DEPTH(ctx, e)                                                                                  \
    do {                                                                                                               \
        if (ctx->recursion_depth++ >= MAX_RECURSION_DEPTH)                                                             \
            _error(ctx, MaxRecursionDepthError, "Maximum recursion depth of %d exceeded", MAX_RECURSION_DEPTH);        \
        e;                                                                                                             \
        ctx->recursion_depth--;                                                                                        \
    } while (0)

static Number expression(EvalContext *ctx);

Number eval(EvalContext *ctx, const char *expr) {
    ctx->curr         = expr;
    ctx->error_msg[0] = '\0';
    ctx->error_type   = NoError;
    if (setjmp(ctx->env) == 0) {
        ctx->last_result = expression(ctx);
        if (*ctx->curr != '\0') {
            ctx->error_type = SyntaxError;
            snprintf(ctx->error_msg, EVAL_ERROR_MSG_LEN * sizeof(char), "Unexpected character '%c'", *ctx->curr);
            return E_NAN;
        }
        table_set_number(ctx->table, "ans", ctx->last_result);
        return ctx->last_result;
    } else {
        return E_NAN;
    }
}

void eval_scope_new(EvalContext *ctx) {
    Table *new = xmalloc(Table);
    table_init(new);
    new->enclosing = ctx->table;
    ctx->table     = new;
}

void eval_scope_end(EvalContext *ctx) {
    Table *enclosing = ctx->table->enclosing;
    table_clear(ctx->table);
    free(ctx->table);
    ctx->table = enclosing;
}

void eval_init(EvalContext *ctx) {
    ctx->error_msg[0]    = '\0';
    ctx->error_type      = NoError;
    ctx->last_result     = E_NAN;
    ctx->curr            = NULL;
    ctx->recursion_depth = 0;
    ctx->table           = NULL;

    eval_scope_new(ctx);

    /* Constants */
    table_set_number(ctx->table, "pi", E_PI);
    table_set_number(ctx->table, "e", E_E);
    table_set_number(ctx->table, "tau", E_TAU);
    table_set_number(ctx->table, "phi", E_PHI);
    table_set_number(ctx->table, "deg", E_DEG);
    table_set_number(ctx->table, "rad", E_RAD);
    table_set_number(ctx->table, "c", E_C);
    table_set_number(ctx->table, "na", E_NA);

    /* Trigonometric */
    table_set_cfunction(ctx->table, "sin", (void *)e_sin, 1);
    table_set_cfunction(ctx->table, "cos", (void *)e_cos, 1);
    table_set_cfunction(ctx->table, "tan", (void *)e_tan, 1);
    table_set_cfunction(ctx->table, "asin", (void *)e_asin, 1);
    table_set_cfunction(ctx->table, "arcsin", (void *)e_asin, 1);
    table_set_cfunction(ctx->table, "acos", (void *)e_acos, 1);
    table_set_cfunction(ctx->table, "arccos", (void *)e_acos, 1);
    table_set_cfunction(ctx->table, "atan", (void *)e_atan, 1);
    table_set_cfunction(ctx->table, "arctan", (void *)e_atan, 1);
    table_set_cfunction(ctx->table, "atan2", (void *)e_atan2, 2);
    table_set_cfunction(ctx->table, "arctan2", (void *)e_atan2, 2);

    /* Hyperbolic */
    table_set_cfunction(ctx->table, "sinh", (void *)e_sinh, 1);
    table_set_cfunction(ctx->table, "cosh", (void *)e_cosh, 1);
    table_set_cfunction(ctx->table, "tanh", (void *)e_tanh, 1);
    table_set_cfunction(ctx->table, "asinh", (void *)e_asinh, 1);
    table_set_cfunction(ctx->table, "arcsinh", (void *)e_asinh, 1);
    table_set_cfunction(ctx->table, "acosh", (void *)e_acosh, 1);
    table_set_cfunction(ctx->table, "arccosh", (void *)e_acosh, 1);
    table_set_cfunction(ctx->table, "atanh", (void *)e_atanh, 1);
    table_set_cfunction(ctx->table, "arctanh", (void *)e_atanh, 1);

    /* Exponential and Logarithmic */
    table_set_cfunction(ctx->table, "exp", (void *)e_exp, 1);
    table_set_cfunction(ctx->table, "exp2", (void *)e_exp2, 1);
    table_set_cfunction(ctx->table, "ln", (void *)e_log, 1);
    table_set_cfunction(ctx->table, "log", (void *)e_log, 1);
    table_set_cfunction(ctx->table, "log10", (void *)e_log10, 1);
    table_set_cfunction(ctx->table, "log2", (void *)e_log2, 1);
    table_set_cfunction(ctx->table, "logb", (void *)e_logb, 2);

    /* Rounding */
    table_set_cfunction(ctx->table, "round", (void *)e_round, 1);
    table_set_cfunction(ctx->table, "floor", (void *)e_floor, 1);
    table_set_cfunction(ctx->table, "ceil", (void *)e_ceil, 1);
    table_set_cfunction(ctx->table, "trunc", (void *)e_trunc, 1);

    /* Power and Root */
    table_set_cfunction(ctx->table, "sqrt", (void *)e_sqrt, 1);
    table_set_cfunction(ctx->table, "cbrt", (void *)e_cbrt, 1);
    table_set_cfunction(ctx->table, "pow", (void *)e_pow, 2);
    table_set_cfunction(ctx->table, "hypot", (void *)e_hypot, 2);

    /* General */
    table_set_cfunction(ctx->table, "abs", (void *)e_abs, 1);
    table_set_cfunction(ctx->table, "mod", (void *)e_mod, 2);
    table_set_cfunction(ctx->table, "remainder", (void *)e_remainder, 2);
    table_set_cfunction(ctx->table, "fma", (void *)e_fma, 3);
    table_set_function(ctx->table, "max", (Function)e_max, 0, MAX_FUNCTION_ARGS);
    table_set_function(ctx->table, "min", (Function)e_min, 0, MAX_FUNCTION_ARGS);
    table_set_cfunction(ctx->table, "factorial", (void *)e_factorial, 1);
    table_set_cfunction(ctx->table, "gamma", (void *)e_tgamma, 1);
    table_set_cfunction(ctx->table, "lgamma", (void *)e_lgamma, 1);
    table_set_cfunction(ctx->table, "erf", (void *)e_erf, 1);
    table_set_cfunction(ctx->table, "erfc", (void *)e_erfc, 1);
    table_set_cfunction(ctx->table, "j0", (void *)e_j0, 1);
    table_set_cfunction(ctx->table, "j1", (void *)e_j1, 1);
    table_set_cfunction(ctx->table, "jn", (void *)e_jn, 2);
    table_set_cfunction(ctx->table, "y0", (void *)e_y0, 1);
    table_set_cfunction(ctx->table, "y1", (void *)e_y1, 1);
    table_set_cfunction(ctx->table, "yn", (void *)e_yn, 2);

    eval_scope_new(ctx); // ? separate locals and globals
}

void eval_end(EvalContext *ctx) {
    ctx->error_msg[0]    = '\0';
    ctx->error_type      = NoError;
    ctx->last_result     = E_NAN;
    ctx->curr            = NULL;
    ctx->recursion_depth = 0;
    eval_scope_end(ctx); // ? local scope
    eval_scope_end(ctx); // ? global scope
}

char *eval_stringify(char *buff, size_t len, Number num) {
    static char default_buff[EVAL_STRINGIFY_BUFFSIZE];
    if (!buff) {
        buff = default_buff;
        len  = EVAL_STRINGIFY_BUFFSIZE;
    }
    len       = e_snprintf(buff, len * sizeof(char), "%.*" E_NUMBER_FMT, E_NUMBER_DIG, num);
    char *dot = strchr(buff, '.');
    if (dot) {
        char *end = buff + len - 1;
        while (end > dot && *end == '0') *(end--) = '\0';
        if (end == dot) *end = '\0';
    }
    return buff;
}

static __attribute__((noreturn)) void _error(EvalContext *ctx, EvalErrorType error_type, const char *fmt, ...) {
    ctx->error_type = error_type;
    va_list args;
    va_start(args, fmt);
    vsnprintf(ctx->error_msg, EVAL_ERROR_MSG_LEN * sizeof(char), fmt, args);
    va_end(args);
    longjmp(ctx->env, (int)error_type);
}

static inline void skipspace(EvalContext *ctx) {
    while (isspace(*ctx->curr)) ctx->curr++;
}

static void eatchar(EvalContext *ctx, char c) {
    skipspace(ctx);
    if (*ctx->curr == c)
        ctx->curr++;
    else
        _error(ctx, SyntaxError, "Expected '%c', got '%s'", c, _c_or_eof(*ctx->curr));
}

static Number number(EvalContext *ctx) {
    char *end;
    Number result = e_strtonum(ctx->curr, &end);
    ctx->curr     = end;
    return result;
}

static Number grouping(EvalContext *ctx) {
    ctx->curr++; // eat the parenthesize '('
    Number result = expression(ctx);
    eatchar(ctx, ')');
    return result;
}

static Number absolute(EvalContext *ctx) {
    ctx->curr++; // eat the absolute value pipe '|'
    Number result = expression(ctx);
    eatchar(ctx, '|');
    return e_abs(result);
}

static TableEntry *identifier(EvalContext *ctx) {
    const char *start = ctx->curr;
    while (isalnum(*ctx->curr) || *ctx->curr == '_') ctx->curr++;
    size_t len = ctx->curr - start;
    char name[len + 1];
    memcpy(name, start, len);
    name[len]         = '\0';
    TableEntry *entry = table_lookup(ctx->table, name, true);
    if (entry) return entry;
    _error(ctx, UndefinedVariableError, "Undefined variable '%s'", name);
}

static Number function_call(EvalContext *ctx, TableEntry *entry) {
    Function func = entry->func;
    Number args[entry->max_args];
    size_t arg_count = 0;
    skipspace(ctx);
    if (*ctx->curr == '(') {
        ctx->curr++;
        skipspace(ctx);
        if (*ctx->curr != ')') {
            while (true) {
                if (arg_count >= entry->max_args)
                    _error(
                        ctx, FunctionArgumentCountError, "Function '%s' expects at most %zu arguments, got %zu",
                        entry->name, entry->max_args, arg_count + 1
                    );
                args[arg_count++] = expression(ctx);
                skipspace(ctx);
                if (*ctx->curr == ')') break;
                eatchar(ctx, ',');
            }
        }
        eatchar(ctx, ')');
    } else
        args[arg_count++] = expression(ctx); // ? function with one arg can be called without parenthesize
    if (arg_count > entry->max_args)
        _error(
            ctx, FunctionArgumentCountError, "Function '%s' expects at most %zu arguments, got %zu", entry->name,
            entry->max_args, arg_count
        );
    if (arg_count < entry->min_args)
        _error(
            ctx, FunctionArgumentCountError, "Function '%s' expects at least %zu arguments, got %zu", entry->name,
            entry->min_args, arg_count
        );
    Number result;
    if (entry->type == ENTRY_TYPE_FUNCTION)
        result = func(args, arg_count);
    else
        switch (arg_count) {
            case 0: result = ((Number (*)(void))entry->cfunc)(); break;
            case 1: result = ((Number (*)(Number))entry->cfunc)(args[0]); break;
            case 2: result = ((Number (*)(Number, Number))entry->cfunc)(args[0], args[1]); break;
            case 3: result = ((Number (*)(Number, Number, Number))entry->cfunc)(args[0], args[1], args[2]); break;
            default: _error(ctx, ImplementationError, "C function '%s' supports up to 3 arguments", entry->name);
        }
    if (e_isnan(result))
        _error(ctx, FunctionArgumentRangeError, "Function '%s' got argument out of range", entry->name);
    return result;
}

static Number primary(EvalContext *ctx) {
    skipspace(ctx);
    if (isdigit(*ctx->curr) || (*ctx->curr == '.' && isdigit(*(ctx->curr + 1)))) return number(ctx);
    if (*ctx->curr == '(') return grouping(ctx);
    if (*ctx->curr == '|') return absolute(ctx);
    if (isalpha(*ctx->curr) || *ctx->curr == '_') {
        TableEntry *entry = identifier(ctx);
        if (entry->type == ENTRY_TYPE_NUMBER) return entry->num;
        if (entry->type == ENTRY_TYPE_FUNCTION || entry->type == ENTRY_TYPE_CFUNCTION) return function_call(ctx, entry);
    }
    _error(ctx, SyntaxError, "Expected a number or parenthesized expression, got '%s'", _c_or_eof(*ctx->curr));
}

static Number factorial(EvalContext *ctx) {
    Number result = primary(ctx);
    skipspace(ctx);
    while (*ctx->curr == '!' && *(ctx->curr + 1) != '=') {
        result = e_factorial(result);
        ctx->curr++;
        skipspace(ctx);
    }
    return result;
}

static Number exponent(EvalContext *ctx) {
    Number result = factorial(ctx);
    skipspace(ctx);
    if (*ctx->curr == '^') {
        ctx->curr++;
        CHECK_RECURSION_DEPTH(ctx, result = e_pow(result, exponent(ctx)));
    }
    return result;
}

static Number unary(EvalContext *ctx) {
    skipspace(ctx);
    bool is_negative = false;
    while (*ctx->curr == '+' || *ctx->curr == '-') {
        if (*ctx->curr == '-') is_negative = !is_negative;
        ctx->curr++;
        skipspace(ctx);
    }
    Number result = exponent(ctx);
    return is_negative ? -result : result;
}

static Number factor(EvalContext *ctx) {
    Number result = unary(ctx);
    Number denom;
    char errorbuff[EVAL_STRINGIFY_BUFFSIZE];
    while (true) {
        skipspace(ctx);
        switch (*ctx->curr) {
            case '*':
                ctx->curr++;
                result *= unary(ctx);
                break;
            case '/':
                ctx->curr++;
                if ((denom = unary(ctx)) == 0.0Q)
                    _error(
                        ctx, DivisionByZeroError, "Division by zero (%s / %s)",
                        eval_stringify(errorbuff, EVAL_STRINGIFY_BUFFSIZE, result), eval_stringify(NULL, 0, denom)
                    );
                result /= denom;
                break;
            case '%':
                ctx->curr++;
                if ((denom = unary(ctx)) == 0.0Q)
                    _error(
                        ctx, ModuloByZeroError, "Modulo by zero (%s %% %s)",
                        eval_stringify(errorbuff, EVAL_STRINGIFY_BUFFSIZE, result), eval_stringify(NULL, 0, denom)
                    );
                result = e_mod(result, denom);
                break;
            default:
                if (isalpha(*ctx->curr) || *ctx->curr == '_' || *ctx->curr == '(') // Implicit multiplication
                    result *= primary(ctx);
                else
                    return result;
        }
    }
}

static Number term(EvalContext *ctx) {
    Number result = factor(ctx);
    while (true) {
        skipspace(ctx);
        switch (*ctx->curr) {
            case '+':
                ctx->curr++;
                result += factor(ctx);
                break;
            case '-':
                ctx->curr++;
                result -= factor(ctx);
                break;
            default: return result;
        }
    }
}

static inline bool comparison_eq(Number x, Number y) { return e_abs(x - y) <= COMPARISON_EPSILON; }
static inline bool comparison_le(Number x, Number y) { return x < y || comparison_eq(x, y); }
static inline bool comparison_lt(Number x, Number y) { return x < y && !comparison_eq(x, y); }
static inline bool comparison_ge(Number x, Number y) { return x > y || comparison_eq(x, y); }
static inline bool comparison_gt(Number x, Number y) { return x > y && !comparison_eq(x, y); }

static Number comparison(EvalContext *ctx) {
    Number result = term(ctx);
    while (true) {
        skipspace(ctx);
        if (*ctx->curr == '<')
            result = *++ctx->curr == '=' && ++ctx->curr ? comparison_le(result, term(ctx))
                                                        : comparison_lt(result, term(ctx));
        else if (*ctx->curr == '>')
            result = *++ctx->curr == '=' && ++ctx->curr ? comparison_ge(result, term(ctx))
                                                        : comparison_gt(result, term(ctx));
        else if (*ctx->curr == '=' && *(ctx->curr + 1) == '=' && (ctx->curr += 2))
            result = comparison_eq(result, term(ctx));
        else if (*ctx->curr == '!' && *(ctx->curr + 1) == '=' && (ctx->curr += 2))
            result = !comparison_eq(result, term(ctx));
        else
            return result;
    }
}

static Number assign(EvalContext *ctx) {
    skipspace(ctx);
    Number result = E_NAN;
    if (isalpha(*ctx->curr) || *ctx->curr == '_') {
        const char *end = ctx->curr;
        while (isalnum(*end) || *end == '_') end++;
        size_t len = end - ctx->curr;
        while (isspace(*end)) end++;
        if (*end == '=' && *(end + 1) != '=') {
            char *name = strndup(ctx->curr, len);
            ctx->curr  = end + 1;
            CHECK_RECURSION_DEPTH(ctx, result = assign(ctx));
            table_set_number(ctx->table, name, result);
            free(name);
        }
    }
    return e_isnan(result) ? comparison(ctx) : result;
}

static Number expression(EvalContext *ctx) {
    Number result;
    CHECK_RECURSION_DEPTH(ctx, result = assign(ctx));
    return result;
}
