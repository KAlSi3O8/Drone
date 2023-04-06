#ifndef __GY_86_H
#define __GY_86_H

#include <stm32f4xx.h>
#include <my_math.h>
#define MPU6050_ADDR    0xD0    //0x68 << 1
#define MS5611_ADDR     0xEE    //0x77 << 1
#define HMC5883L_ADDR   0x3C    //0x1E << 1
#define ANGLETORAD      0.01745f

//MPU6050 Struct Define
typedef struct{
    int16_t AX;
    int16_t AY;
    int16_t AZ;
    int16_t Temp;
    int16_t GX;
    int16_t GY;
    int16_t GZ;
}MPU_DATA;
extern MPU_DATA MPU_Data;
typedef struct{
    float X;
    float Y;
    float Z;
}Acc_DATA;
extern Acc_DATA Acc_Data;
typedef struct{
    float X;
    float Y;
    float Z;
}Gyro_DATA;
extern Gyro_DATA Gyro_Data;
void MPU_Init(void);
void MPU_Read(void);
void MPU_Trans(void);

//HMC5883L Struct Define
typedef struct{
    int16_t X;
    int16_t Y;
    int16_t Z;
}HMC_DATA;
extern HMC_DATA HMC_Data;
typedef struct{
    float X;
    float Y;
    float Z;
}Mag_DATA;
extern Mag_DATA Mag_Data;
void HMC_Init(void);
void HMC_Read(void);
void HMC_Trans(void);

void GY86_adj(void);

#endif
