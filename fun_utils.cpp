#include "fun_utils.h"

namespace fun_utils{
    int clamp(int val, int min, int max){
        if(val < min){
            return min;
        } else if(val > max){
            return max;
        } else {
            return val;
        }
    }

    float clamp(float val, float min, float max){
        if(val < min){
            return min;
        } else if(val > max){
            return max;
        } else {
            return val;
        }
    }

    float clamp(float val, int min, int max){
        if(val < min){
            return min;
        } else if(val > max){
            return max;
        } else {
            return val;
        }
    }
    
    int sign(int val){
        if(val < 0){
            return -1;
        } else if(val > 0){
            return 1;
        } else {
            return 0;
        }
    }

    int abs(int val){
        if(val < 0){
            return -val;
        } else {
            return val;
        }
    }

    float abs(float val){
        if(val < 0){
            return -val;
        } else {
            return val;
        }
    }

    bool isBetween(int val, int a, int b){
        // The order of a and b doesn't matter
        if(a > b){
            int temp = a;
            a = b;
            b = temp;
        }
        
        // The value is between a and b or equal to one of them
        if(val >= a && val <= b){
            return true;
        } else {
            return false;
        }
    }
}