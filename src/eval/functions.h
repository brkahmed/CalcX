#ifndef EVAL_FUNCTIONS_H
#define EVAL_FUNCTIONS_H

#include <quadmath.h>

#include "eval.h"

#define NAN   nanq("")
#define E_PI  M_PIq
#define E_E   M_Eq
#define E_TAU 6.283185307179586476925286766559006Q

Number min(Number nums[], size_t len);
Number max(Number nums[], size_t len);

#endif // EVAL_FUNCTIONS_H
