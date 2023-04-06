#ifndef __MYTIM_H
#define __MYTIM_H

#include <stm32f4xx.h>

#define USE_IT 1

extern int16_t Receiver_Data[];
extern uint8_t flag;

void MY_TIM_Init(void);
void MY_Delay_Init(void);
void MY_Delay(uint32_t time);
void MY_TIMER_Init(void);
#if !USE_IT
void MY_Read_CCR(void);
#endif

#endif
