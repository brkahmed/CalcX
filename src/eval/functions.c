#include "functions.h"

#include <quadmath.h>

Number e_min(Number nums[], size_t len) {
    if (len < 1) return -HUGE_VALQ;
    Number min = nums[0];
    for (size_t i = 1; i < len; i++)
        if (nums[i] < min) min = nums[i];
    return min;
}

Number e_max(Number nums[], size_t len) {
    if (len < 1) return HUGE_VALQ;
    Number max = nums[0];
    for (size_t i = 1; i < len; i++)
        if (nums[i] > max) max = nums[i];
    return max;
}

Number e_logb(Number b, Number x) { return b ? logq(x) / logq(b) : NAN; }

Number e_factorial(Number num) { return tgammaq(num + 1); }

Number e_yn(Number n, Number x) { return ynq((int)n, x); }

Number e_jn(Number n, Number x) { return jnq((int)n, x); }