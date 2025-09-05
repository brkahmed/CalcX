#include "eval.h"

#define __USE_GNU

#include <ctype.h>
#include <float.h>
#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"
#include "table.h"

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
            return NAN;
        }
        table_set_number(&ctx->table, "ans", ctx->last_result);
        return ctx->last_result;
    } else {
        return NAN;
    }
}

void eval_ctx_init(EvalContext *ctx) {
    ctx->error_msg[0]    = '\0';
    ctx->error_type      = NoError;
    ctx->last_result     = NAN;
    ctx->curr            = NULL;
    ctx->recursion_depth = 0;
    table_init(&ctx->table);

    /* Constants */
    table_set_number(&ctx->table, "pi", M_PIl);
    table_set_number(&ctx->table, "e", M_El);

    table_set_function(&ctx->table, "max", (Function)max, 0, MAX_FUNCTION_ARGS);
    table_set_function(&ctx->table, "min", (Function)min, 0, MAX_FUNCTION_ARGS);
    table_set_cfunction(&ctx->table, "abs", (void *)fabsl, 1);
}

char *eval_stringify(char *buff, size_t len, Number num) {
    static char default_buff[128];
    if (!buff) {
        buff = default_buff;
        len  = 128;
    }
    len       = snprintf(buff, len * sizeof(char), "%.*Lf", LDBL_DIG, num);
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
    Number result = strtold(ctx->curr, &end);
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
    return fabsl(result);
}

static TableEntry *identifier(EvalContext *ctx) {
    const char *start = ctx->curr;
    while (isalnum(*ctx->curr) || *ctx->curr == '_') ctx->curr++;
    size_t len = ctx->curr - start;
    char name[len + 1];
    memcpy(name, start, len);
    name[len]         = '\0';
    TableEntry *entry = table_lookup(&ctx->table, name);
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
                        entry->name, entry->max_args, arg_count
                    );
                args[arg_count++] = expression(ctx);
                skipspace(ctx);
                if (*ctx->curr == ')') break;
                eatchar(ctx, ',');
            }
        }
        eatchar(ctx, ')');
    } else
        args[arg_count++] = expression(ctx);
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
    if (isnanl(result)) _error(ctx, FunctionArgumentRangeError, "Function '%s' got argument out of range", entry->name);
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
    while (*ctx->curr == '!') {
        result = tgammal(result + 1);
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
        CHECK_RECURSION_DEPTH(ctx, result = powl(result, exponent(ctx)));
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
    while (true) {
        skipspace(ctx);
        switch (*ctx->curr) {
            case '*':
                ctx->curr++;
                result *= unary(ctx);
                break;
            case '/':
                ctx->curr++;
                if ((denom = unary(ctx)) == 0.0L)
                    _error(ctx, DivisionByZeroError, "Division by zero (%Lf / %Lf)", result, denom);
                result /= denom;
                break;
            case '%':
                ctx->curr++;
                if ((denom = unary(ctx)) == 0.0L)
                    _error(ctx, ModuloByZeroError, "Modulo by zero (%Lf %% %Lf)", result, denom);
                result = fmodl(result, denom);
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

static Number expression(EvalContext *ctx) {
    Number result;
    CHECK_RECURSION_DEPTH(ctx, result = term(ctx));
    return result;
}
