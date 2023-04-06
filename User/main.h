#ifndef __MAIN_H
#define __MAIN_H

#include <stm32f4xx.h>
#include <stdio.h>
#include "MyUART.h"
#include "MyI2C.h"
#include "MyTIM.h"
#include "GY-86.h"
#include "Motor.h"
#include "OLED.h"
#include "zimo.h"
#include "ESP8266.h"
#include "anotc.h"
#include "Madgwick.h"

#define PLL_M   8
#define PLL_N   336
#define PLL_Q   7
#define PLL_P   4

void MY_System_Config(void);
void MY_Delay(uint32_t time);
void Show_Data(void);
void Show_Name(void);
void Send_Data(void);

#endif
