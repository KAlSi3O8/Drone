#ifndef __MADGWICK_H
#define __MADGWICK_H

#include "GY-86.h"
#include "my_math.h"
#include "math.h"

struct EULRA_Struct{
    float yaw;
    float pitch;
    float roll;
};
extern struct EULRA_Struct eulra;

void My_Madgwick_Update(float ax, float ay, float az,\
                        float gx, float gy, float gz,\
                        float mx, float my, float mz);

#endif
