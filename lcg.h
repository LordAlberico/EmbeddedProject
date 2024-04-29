#ifndef LCG_H
#define LCG_H

#include <cstdlib>
#include "arch/common/drivers/stm32_hardware_rng.h"

void setPseudoSeed(int newSeed);

int pseudoRNG(int lower, int upper);

int hardwareRNG(int lower, int upper);

float hardwareRNG(float lower, float upper);

float hardwareRNG(double lower, double upper);

#endif