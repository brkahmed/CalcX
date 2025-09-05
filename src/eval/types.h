#ifndef EVAL_TYPES_H
#define EVAL_TYPES_H

#include <quadmath.h>
#include <stddef.h>

typedef __float128 Number;
typedef Number (*Function)(Number[], size_t);

#endif // EVAL_TYPES_H
