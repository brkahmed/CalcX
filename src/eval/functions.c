#include "functions.h"

Number e_min(Number nums[], size_t len) {
    if (len < 1) return -E_HUGE_VAL;
    Number min = nums[0];
    for (size_t i = 1; i < len; i++)
        if (nums[i] < min) min = nums[i];
    return min;
}

Number e_max(Number nums[], size_t len) {
    if (len < 1) return E_HUGE_VAL;
    Number max = nums[0];
    for (size_t i = 1; i < len; i++)
        if (nums[i] > max) max = nums[i];
    return max;
}

Number e_logb(Number b, Number x) { return b ? e_log(x) / e_log(b) : E_NAN; }

Number e_factorial(Number num) { return e_tgamma(num + 1); }

Number e_yn(Number n, Number x) { return e_yn_((int)n, x); }

Number e_jn(Number n, Number x) { return e_jn_((int)n, x); }