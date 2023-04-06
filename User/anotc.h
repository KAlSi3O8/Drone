#ifndef __ANOTC_H
#define __ANOTC_H

#include <stm32f4xx.h>
#include "GY-86.h"
#include "MyUART.h"

void Anotc_Send_Eulra(float roll, float pitch, float yaw);
void Anotc_Send_Raw(int16_t AX, int16_t AY, int16_t AZ, int16_t GX, int16_t GY, int16_t GZ, int16_t MX, int16_t MY, int16_t MZ);
void Anotc_Send_PID(float PIDRoll, float PIDPitch, float PIDYaw);

#endif
