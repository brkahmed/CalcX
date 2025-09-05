#ifndef EVAL_H
#define EVAL_H

#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>

#include <quadmath.h>

#include "table.h"
#include "types.h"

#define EVAL_ERROR_MSG_LEN      512
#define EVAL_STRINGIFY_BUFFSIZE FLT128_MAX_10_EXP + FLT128_DIG + 10
#define MAX_FUNCTION_ARGS       1024
#define MAX_IDENTIFIER_LEN      64
#define MAX_RECURSION_DEPTH     1024

typedef enum {
    NoError,
    DivisionByZeroError,
    ModuloByZeroError,
    UndefinedVariableError,
    FunctionArgumentCountError,
    FunctionArgumentRangeError,
    ImplementationError,
    MaxRecursionDepthError,
    SyntaxError,
} EvalErrorType;

typedef struct {
    char error_msg[EVAL_ERROR_MSG_LEN];
    EvalErrorType error_type;
    Number last_result;
    Table table;

    const char *curr;
    jmp_buf env;
    size_t recursion_depth;
} EvalContext;

Number eval(EvalContext *ctx, const char *expr);
void eval_ctx_init(EvalContext *ctx);
char *eval_stringify(char *buff, size_t len, Number num);

#endif