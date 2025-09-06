#ifndef EVAL_FUNCTIONS_H
#define EVAL_FUNCTIONS_H

#include <quadmath.h>

#include "types.h"

#define NAN   nanq("")
#define E_PI  M_PIq
#define E_E   M_Eq
#define E_TAU 6.283185307179586476925286766559006Q
#define E_PHI 1.618033988749894848204586834365638Q
#define E_DEG 0.017453292519943295769236907684886Q
#define E_RAD 57.295779513082320876798154814105172Q
#define E_C   299792458.0Q   /* speed of light in m/s */
#define E_NA  6.02214076E23Q /* avogadro number in mol-1*/

Number e_min(Number[], size_t);
Number e_max(Number[], size_t);
Number e_logb(Number, Number);
Number e_factorial(Number);
Number e_yn(Number, Number);
Number e_jn(Number, Number);

#endif // EVAL_FUNCTIONS_H
