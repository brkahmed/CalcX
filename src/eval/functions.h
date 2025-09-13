#ifndef EVAL_FUNCTIONS_H
#define EVAL_FUNCTIONS_H

#include <stddef.h>

#include "types.h"

#ifdef HAVE_QUADMATH
#include <quadmath.h>
#define E_NAN         nanq("")
#define E_PI          M_PIq
#define E_E           M_Eq
#define E_TAU         6.283185307179586476925286766559006Q
#define E_PHI         1.618033988749894848204586834365638Q
#define E_DEG         0.017453292519943295769236907684886Q
#define E_RAD         57.295779513082320876798154814105172Q
#define E_C           299792458.0Q   /* speed of light in m/s */
#define E_NA          6.02214076E23Q /* avogadro number in mol-1*/
#define E_FUNCTION(f) f##q
#define e_snprintf    quadmath_snprintf
#else
#include <math.h>
#define E_NAN         nan("")
#define E_PI          M_PI
#define E_E           M_E
#define E_TAU         6.283185307179586476925286766559006L
#define E_PHI         1.618033988749894848204586834365638L
#define E_DEG         0.017453292519943295769236907684886L
#define E_RAD         57.295779513082320876798154814105172L
#define E_C           299792458.0L   /* speed of light in m/s */
#define E_NA          6.02214076E23L /* avogadro number in mol-1*/
#define E_FUNCTION(f) f##l
#define e_snprintf    snprintf
#endif

Number e_min(Number[], size_t);
Number e_max(Number[], size_t);
Number e_logb(Number, Number);
Number e_factorial(Number);
Number e_yn(Number, Number);
Number e_jn(Number, Number);

/* Trigonometric */
#define e_sin   E_FUNCTION(sin)
#define e_cos   E_FUNCTION(cos)
#define e_tan   E_FUNCTION(tan)
#define e_asin  E_FUNCTION(asin)
#define e_acos  E_FUNCTION(acos)
#define e_atan  E_FUNCTION(atan)
#define e_atan2 E_FUNCTION(atan2)

/* Hyperbolic */
#define e_sinh  E_FUNCTION(sinh)
#define e_cosh  E_FUNCTION(cosh)
#define e_tanh  E_FUNCTION(tanh)
#define e_asinh E_FUNCTION(asinh)
#define e_acosh E_FUNCTION(acosh)
#define e_atanh E_FUNCTION(atanh)

/* Exponential & Logarithmic */
#define e_exp   E_FUNCTION(exp)
#define e_exp2  E_FUNCTION(exp2)
#define e_log   E_FUNCTION(log)
#define e_log10 E_FUNCTION(log10)
#define e_log2  E_FUNCTION(log2)

/* Rounding */
#define e_round E_FUNCTION(round)
#define e_floor E_FUNCTION(floor)
#define e_ceil  E_FUNCTION(ceil)
#define e_trunc E_FUNCTION(trunc)

/* Power & Root */
#define e_sqrt  E_FUNCTION(sqrt)
#define e_cbrt  E_FUNCTION(cbrt)
#define e_pow   E_FUNCTION(pow)
#define e_hypot E_FUNCTION(hypot)

/* General */
#define e_abs       E_FUNCTION(fabs)
#define e_mod       E_FUNCTION(fmod)
#define e_remainder E_FUNCTION(remainder)
#define e_fma       E_FUNCTION(fma)
#define e_tgamma    E_FUNCTION(tgamma)
#define e_lgamma    E_FUNCTION(lgamma)
#define e_erf       E_FUNCTION(erf)
#define e_erfc      E_FUNCTION(erfc)
#define e_j0        E_FUNCTION(j0)
#define e_j1        E_FUNCTION(j1)
#define e_jn_        E_FUNCTION(jn)
#define e_y0        E_FUNCTION(y0)
#define e_y1        E_FUNCTION(y1)
#define e_yn_        E_FUNCTION(yn)

#define e_isnan E_FUNCTION(isnan)

#endif // EVAL_FUNCTIONS_H
