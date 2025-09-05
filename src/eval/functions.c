#include "functions.h"

#include <math.h>

Number min(Number nums[], size_t len) {
    if (len < 1) return -INFINITY;
    Number min = nums[0];
    for (size_t i = 1; i < len; i++)
        if (nums[i] < min) min = nums[i];
    return min;
}

Number max(Number nums[], size_t len) {
    if (len < 1) return INFINITY;
    Number max = nums[0];
    for (size_t i = 1; i < len; i++)
        if (nums[i] > max) max = nums[i];
    return max;
}
