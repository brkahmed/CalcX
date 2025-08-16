#include <math.h>
#include <stdlib.h>

#define M_TAU 6.283185307179586
#define M_PHI 1.618033988749895
#define M_DEG 0.017453292519943295
#define M_RAD 57.29577951308232

static double factorial(double x) {
    if (x == 0) return 1;
    if (x > 0 && trunc(x) == x) {
        double result = x--;
        while (x > 1) result *= x--;
        return result;
    }
    return tgamma(x + 1);
}

static double min(double nums[], size_t len) {
    if (len < 1) return -INFINITY;
    double min = nums[0];
    for (size_t i = 1; i < len; i++)
        if (nums[i] < min) min = nums[i];
    return min;
}

static double max(double nums[], size_t len) {
    if (len < 1) return INFINITY;
    double max = nums[0];
    for (size_t i = 1; i < len; i++)
        if (nums[i] > max) max = nums[i];
    return max;
}
