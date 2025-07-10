#ifndef EVAL_EXPR_IN_ONE_PASS_H
#define EVAL_EXPR_IN_ONE_PASS_H


#include "../include/error.h"

#define MAX_IDENTIFIER_LEN (size_t) 64
#define MAX_FUNCTION_ARGS (size_t) 1024

double eval_expr(const char *expr);


extern error_handler eval_expr_err_handler;

enum ErrorType {
    SyntaxError = 1, DivisionByZeroError, MoreThanMaxError, UnsupportedFunctionError, IncorrectArgumentCountError,
};


#endif