#include "anotc.h"

//Send PID Data
void Anotc_Send_PID(float PIDRoll, float PIDPitch, float PIDYaw){
    int cnt;
    short roll, pitch, yaw;
    unsigned char flame[12] = {0xAA, 0xFF, 0x0A, 6};
    roll = (short)(PIDRoll * 100.0f);
    pitch = (short)(PIDPitch * 100.0f);
    yaw = (short)(PIDYaw * 100.0f);
    flame[4] = roll & 0xFF;
    flame[5] = (roll >> 8) & 0xFF;
    flame[6] = pitch & 0xFF;
    flame[7] = (pitch >> 8) & 0xFF;
    flame[8] = yaw & 0xFF;
    flame[9] = (yaw >> 8) & 0xFF;
    for(cnt = 0, flame[10] = 0; cnt <= 9; cnt++){
        flame[10] += flame[cnt];
        flame[11] += flame[10];
    }
    MY_UART_Send(flame, 12);
}

//Send Eulra Flame to Anotc Software
void Anotc_Send_Eulra(float roll, float pitch, float yaw){
    //Data process
    int cnt;
    short int16_roll, int16_pitch, int16_yaw;
    unsigned char flame[13] = {0xAA, 0xFF, 0x03, 7};
    int16_roll  = (short)(roll  * 100.0f);
    int16_pitch = (short)(pitch * 100.0f);
    int16_yaw   = (short)(yaw   * 100.0f);
    flame[4] = int16_roll & 0xFF;
    flame[5] = (int16_roll >> 8) & 0xFF;
    flame[6] = int16_pitch & 0xFF;
    flame[7] = (int16_pitch >> 8) & 0xFF;
    flame[8] = int16_yaw & 0xFF;
    flame[9] = (int16_yaw >> 8) &0xFF;
    flame[10] = 0x01;
    for(cnt = 0, flame[11] = 0; cnt <= 10; cnt++){
        flame[11] += flame[cnt];
        flame[12] += flame[11];
    }
    MY_UART_Send(flame, 13);
}

//Send Raw data to Anotc Software
void Anotc_Send_Raw(int16_t AX, int16_t AY, int16_t AZ, int16_t GX, int16_t GY, int16_t GZ, int16_t MX, int16_t MY, int16_t MZ){
    int cnt;
    unsigned char flame[20] = {0xAA, 0xFF, 0x01, 13};
    flame[4] = AX & 0xFF;
    flame[5] = (AX >> 8) & 0xFF;
    flame[6] = AY & 0xFF;
    flame[7] = (AY >> 8) & 0xFF;
    flame[8] = AZ & 0xFF;
    flame[9] = (AZ >> 8) & 0xFF;
    flame[10] = GX & 0xFF;
    flame[11] = (GX >> 8) & 0xFF;
    flame[12] = GY & 0xFF;
    flame[13] = (GY >> 8) & 0xFF;
    flame[14] = GZ & 0xFF;
    flame[15] = (GZ >> 8) & 0xFF;
    flame[16] = 0x01;
    for(cnt = 0, flame[17] = 0; cnt <= 16; cnt++){
        flame[17] += flame[cnt];
        flame[18] += flame[17];
    }
    MY_UART_Send(flame, 19);
    
    flame[2] = 0x02;
    flame[3] = 14;
    flame[4] = MX & 0xFF;
    flame[5] = (MX >> 8) & 0xFF;
    flame[6] = MY & 0xFF;
    flame[7] = (MY >> 8) & 0xFF;
    flame[8] = MZ & 0xFF;
    flame[9] = (MZ >> 8) & 0xFF;
    flame[10] = 0;
    flame[11] = 0;
    flame[12] = 0;
    flame[13] = 0;
    flame[14] = 0;
    flame[15] = 0;
    flame[16] = 0;
    flame[17] = 0x01;
    for(cnt = 0, flame[18] = 0; cnt <= 17; cnt++){
        flame[18] += flame[cnt];
        flame[19] += flame[18];
    }
    MY_UART_Send(flame, 20);
}
