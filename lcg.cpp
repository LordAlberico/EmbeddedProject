#include "lcg.h"

using namespace miosix;
using namespace std;

// Linear congruential generator seed
static long long seed = 12345;

void setPseudoSeed(int newSeed){
    srand(newSeed);
}

int pseudoRNG(int lower, int upper){
    int result = lower + (rand() % (upper - lower + 1));
    return result;
}

int hardwareRNG(int lower, int upper) {
    seed = HardwareRng::instance().get();

    // Scale to the desired range
    int result = lower + (seed % (upper - lower + 1));
    return result;
}

float hardwareRNG(float lower, float upper) {
    int intResult = hardwareRNG(0, 0x7FFFFFFF);
    float result = (float)intResult / 0x7FFFFFFF;
    return lower + result * (upper - lower);
}

// Overload the function to accept double. Force the conversion to float
float hardwareRNG(double lower, double upper) {
    return hardwareRNG((float)lower, (float)upper);
}