#ifndef EVAL_TYPES_H
#define EVAL_TYPES_H

#include <stddef.h>

#ifdef HAVE_QUADMATH
#include <quadmath.h>
typedef __float128 Number;
#define E_HUGE_VAL HUGE_VALQ
#define E_NUMBER_MAX_10_EXP FLT128_MAX_10_EXP
#define E_NUMBER_DIG        FLT128_DIG
#define E_NUMBER_FMT        "Qf"
#define e_strtonum          strtoflt128
#else
#include <float.h>
typedef long double Number;
#define E_HUGE_VAL HUGE_VALF
#define E_NUMBER_MAX_10_EXP LDBL_MAX_10_EXP
#define E_NUMBER_DIG        LDBL_DIG
#define E_NUMBER_FMT        "Lf"
#define e_strtonum          strtold
#endif

typedef Number (*Function)(Number[], size_t);

#endif // EVAL_TYPES_H
