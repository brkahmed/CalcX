#include <stdlib.h>
#include <math.h>


static double factorial(double x) {
    if (trunc(x) != x) return tgamma(x + 1);
    if (x == 0) return 1;
    double result = x--;
    while (x > 1)
        result *= x--;
    return result;
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
