#ifndef EVAL_H
#define EVAL_H

#include <stddef.h>

#define EVAL_ERROR_MSG_LEN  512
#define MAX_FUNCTION_ARGS   1024
#define MAX_IDENTIFIER_LEN  64
#define MAX_RECURSION_DEPTH 1024

typedef long double Number;
typedef Number (*Function)(Number[], size_t);

Number eval(const char *expr);

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

extern char eval_error_msg[EVAL_ERROR_MSG_LEN];
extern EvalErrorType eval_error_type;
extern Number eval_last_result;

#endif