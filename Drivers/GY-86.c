#include "GY-86.h"
#include "MyI2C.h"
#include "MyTIM.h"

#define BUFSIZE 10

MPU_DATA MPU_Data, MPU_offset = {0};
HMC_DATA HMC_Data;
Acc_DATA  Acc_Data;
Gyro_DATA Gyro_Data;
Mag_DATA  Mag_Data;
int16_t Acc_filte[3][BUFSIZE] = {0}, Mag_filte[3][BUFSIZE] = {0};
uint32_t MPU_cnt = 0, HMC_cnt = 0;

//GY-86 adjusting
void GY86_adj(void){
    uint32_t cnt;
    int32_t Ax = 0, Ay = 0, Az = 0, Gx = 0, Gy = 0, Gz = 0;
    for(cnt = BUFSIZE; cnt > 0 ; cnt--){
        MPU_Read();
        HMC_Read();
    }
    for(cnt = 0; cnt < 100; cnt++){
        MPU_Read();
        Ax += MPU_Data.AX;
        Ay += MPU_Data.AY;
        Az += MPU_Data.AZ - 0x4000;
        Gx += MPU_Data.GX;
        Gy += MPU_Data.GY;
        Gz += MPU_Data.GZ;
    }
    MPU_offset.AX = Ax / 100;
    MPU_offset.AY = Ay / 100;
    MPU_offset.AZ = Az / 100;
    MPU_offset.GX = Gx / 100;
    MPU_offset.GY = Gy / 100;
    MPU_offset.GZ = Gz / 100;
}

//MPU6050 Init
void MPU_Init(void){
    MY_I2C_Send(MPU6050_ADDR, (uint32_t)"\x6B\x80", 2);
    MY_Delay(100);
    MY_I2C_Send(MPU6050_ADDR, (uint32_t)"\x6B\x01", 2);
    MY_I2C_Send(MPU6050_ADDR, (uint32_t)"\x19\x00\x13\x10\x00", 5);
    MY_I2C_Send(MPU6050_ADDR, (uint32_t)"\x37\x02", 2);
}

//MPU6050 Read Data
void MPU_Read(void){
    uint8_t temp[14] = {0};
    int32_t sum;
    uint32_t cnt;
    
    MY_I2C_Receive(MPU6050_ADDR, 0x3B, temp, 14);
    Acc_filte[0][MPU_cnt] = (int16_t)((temp[0] << 8) + temp[1] - MPU_offset.AX);
    Acc_filte[1][MPU_cnt] = (int16_t)((temp[2] << 8) + temp[3] - MPU_offset.AY);
    Acc_filte[2][MPU_cnt] = (int16_t)((temp[4] << 8) + temp[5] - MPU_offset.AZ);
    MPU_Data.Temp = (3653 + (int16_t)((temp[6] << 8) | temp[7]) *10 /34) /10;
    MPU_Data.GX = (int16_t)((temp[8]  << 8) + temp[9]  - MPU_offset.GX);
    MPU_Data.GY = (int16_t)((temp[10] << 8) + temp[11] - MPU_offset.GY);
    MPU_Data.GZ = (int16_t)((temp[12] << 8) + temp[13] - MPU_offset.GZ);
    MPU_cnt = MPU_cnt<BUFSIZE-1?MPU_cnt+1:0;
    for(cnt = 0, sum = 0; cnt < BUFSIZE; cnt++){
        sum += Acc_filte[0][cnt];
    }
    MPU_Data.AX = sum/BUFSIZE;
    for(cnt = 0, sum = 0; cnt < BUFSIZE; cnt++){
        sum += Acc_filte[1][cnt];
    }
    MPU_Data.AY = sum/BUFSIZE;
    for(cnt = 0, sum = 0; cnt < BUFSIZE; cnt++){
        sum += Acc_filte[2][cnt];
    }
    MPU_Data.AZ = sum/BUFSIZE;
}

//MPU Data normalize
void MPU_Trans(void){
    float Acc_Len = my_sqrt(MPU_Data.AX*MPU_Data.AX + \
                            MPU_Data.AY*MPU_Data.AY + \
                            MPU_Data.AZ*MPU_Data.AZ);
    Acc_Data.X = (float)MPU_Data.AX / Acc_Len;
    Acc_Data.Y = (float)MPU_Data.AY / Acc_Len;
    Acc_Data.Z = (float)MPU_Data.AZ / Acc_Len;
    
    Gyro_Data.X = (float)MPU_Data.GX / 0x8000f * 1000.0f * ANGLETORAD;
    Gyro_Data.Y = (float)MPU_Data.GY / 0x8000f * 1000.0f * ANGLETORAD;
    Gyro_Data.Z = (float)MPU_Data.GZ / 0x8000f * 1000.0f * ANGLETORAD;
}

//HMC5883 Initialize
void HMC_Init(void){
    MY_I2C_Send(HMC5883L_ADDR, (uint32_t)"\x00\x70\xE0\x00", 4);
}

//HMC5883L Read Data
void HMC_Read(void){
    uint8_t temp[6] = {0};
    int32_t sum;
    uint32_t cnt;
    
    MY_I2C_Receive(HMC5883L_ADDR, 0x03, temp, 6);
    Mag_filte[0][HMC_cnt] = (temp[0] << 8) + temp[1];
    Mag_filte[1][HMC_cnt] = (temp[2] << 8) + temp[3];
    Mag_filte[2][HMC_cnt] = (temp[4] << 8) + temp[5];
    HMC_cnt = HMC_cnt<BUFSIZE-1?HMC_cnt+1:0;
    for(cnt = 0, sum = 0; cnt < BUFSIZE; cnt++){
        sum += Mag_filte[0][cnt];
    }
    HMC_Data.X = sum/BUFSIZE;
    for(cnt = 0, sum = 0; cnt < BUFSIZE; cnt++){
        sum += Mag_filte[1][cnt];
    }
    HMC_Data.Z = sum/BUFSIZE;
    for(cnt = 0, sum = 0; cnt < BUFSIZE; cnt++){
        sum += Mag_filte[2][cnt];
    }
    HMC_Data.Y = sum/BUFSIZE;
}

//HMC Data normalize
void HMC_Trans(void){
    float HMC_Len;
    HMC_Len = my_sqrt(HMC_Data.X*HMC_Data.X + \
                      HMC_Data.Y*HMC_Data.Y + \
                      HMC_Data.Z*HMC_Data.Z);
    Mag_Data.X = (float)HMC_Data.X / HMC_Len;
    Mag_Data.Y = (float)HMC_Data.Y / HMC_Len;
    Mag_Data.Z = (float)HMC_Data.Z / HMC_Len;
}

//MS5611 Initialize
void MS_Init(void){
    MY_I2C_Send(MS5611_ADDR, 0x1E, 1);
}
