#ifndef FUN_UTILS_H
#define FUN_UTILS_H

#include "miosix.h"
#include "lis3dsh.h"

namespace fun_utils{
    int clamp(int value, int min, int max);
    float clamp(float value, float min, float max);
    float clamp(float value, int min, int max);
    int sign(int value);
    int abs(int value);
    float abs(float value);
    bool isBetween(int val, int a, int b);
}

#endif