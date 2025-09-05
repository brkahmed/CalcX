#include "eval.h"

#include <ctype.h>
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
#define CHECK_RECURSION_DEPTH(e)                                                                                       \
    do {                                                                                                               \
        if (recursion_depth++ >= MAX_RECURSION_DEPTH)                                                                  \
            _error(MaxRecursionDepthError, "Maximum recursion depth of %d exceeded", MAX_RECURSION_DEPTH);             \
        e;                                                                                                             \
        recursion_depth--;                                                                                             \
    } while (0)

typedef const char **ptr;

char eval_error_msg[EVAL_ERROR_MSG_LEN];
EvalErrorType eval_error_type = NoError;
Number eval_last_result       = 0;
Table eval_table;
bool eval_inited = false;

static jmp_buf eval_env;
static size_t recursion_depth = 0;

static Number expression(ptr expr);
void eval_init(void);

Number eval(const char *expr) {
    if (!eval_inited) eval_init();
    eval_error_msg[0] = '\0';
    eval_error_type   = NoError;
    recursion_depth   = 0;
    if (setjmp(eval_env) == 0) {
        eval_last_result = expression(&expr);
        if (*expr != '\0') {
            eval_error_type = SyntaxError;
            snprintf(eval_error_msg, EVAL_ERROR_MSG_LEN * sizeof(char), "Unexpected character '%c'", *expr);
            return NAN;
        }
        return eval_last_result;
    } else {
        return NAN;
    }
}

void eval_init(void) {
    table_init(&eval_table);
    table_set_const(&eval_table, "pi", M_PI);
    table_set_function(&eval_table, "max", max, 0, MAX_FUNCTION_ARGS);
    table_set_function(&eval_table, "min", min, 0, MAX_FUNCTION_ARGS);
    table_set_cfunction(&eval_table, "abs", fabsl, 1);
    eval_inited = true;
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

static Number number(ptr curr) {
    char *end;
    Number result = strtold(*curr, &end);
    *curr         = end;
    return result;
}

static Number grouping(ptr curr) {
    (*curr)++;
    Number result = expression(curr);
    eatchar(curr, ')');
    return result;
}

static Number absolute(ptr curr) {
    (*curr)++;
    Number result = expression(curr);
    eatchar(curr, '|');
    return fabsl(result);
}

static TableEntry *identifier(ptr curr) {
    const char *start = *curr;
    while (isalnum(**curr) || **curr == '_') (*curr)++;
    size_t len = *curr - start;
    char name[len + 1];
    memcpy(name, start, len);
    name[len]         = '\0';
    TableEntry *entry = table_lookup(&eval_table, name);
    if (entry) return entry;
    _error(UndefinedVariableError, "Undefined variable '%s'", name);
}

static Number function_call(ptr curr, TableEntry *entry) {
    Function func = entry->func;
    Number args[entry->max_args];
    size_t arg_count = 0;
    skipspace(curr);
    if (**curr == '(') {
        (*curr)++;
        skipspace(curr);
        if (**curr != ')') {
            while (true) {
                if (arg_count >= entry->max_args)
                    _error(
                        FunctionArgumentCountError, "Function '%s' expects at most %zu arguments, got %zu", entry->name,
                        entry->max_args, arg_count
                    );
                args[arg_count++] = expression(curr);
                skipspace(curr);
                if (**curr == ')') break;
                eatchar(curr, ',');
            }
        }
        eatchar(curr, ')');
    } else
        args[arg_count++] = expression(curr);
    if (arg_count > entry->max_args)
        _error(
            FunctionArgumentCountError, "Function '%s' expects at most %zu arguments, got %zu", entry->name,
            entry->max_args, arg_count
        );
    if (arg_count < entry->min_args)
        _error(
            FunctionArgumentCountError, "Function '%s' expects at least %zu arguments, got %zu", entry->name,
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
            default: _error(ImplementationError, "C function '%s' supports up to 3 arguments", entry->name);
        }
    if (isnanl(result)) _error(FunctionArgumentRangeError, "Function '%s' got argument out of range");
    return result;
}

static Number primary(ptr curr) {
    skipspace(curr);
    if (isdigit(**curr) || (**curr == '.' && isdigit(*(*curr + 1)))) return number(curr);
    if (**curr == '(') return grouping(curr);
    if (**curr == '|') return absolute(curr);
    if (isalpha(**curr) || **curr == '_') {
        TableEntry *entry = identifier(curr);
        if (entry->type == ENTRY_TYPE_CONST) return entry->constnum;
        if (entry->type == ENTRY_TYPE_NUMBER) return *(entry->num);
        if (entry->type == ENTRY_TYPE_FUNCTION || entry->type == ENTRY_TYPE_CFUNCTION)
            return function_call(curr, entry);
    }
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
            default:
                if (isalpha(**curr) || **curr == '_' || **curr == '(')
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
