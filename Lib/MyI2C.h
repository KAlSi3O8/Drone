#ifndef __MYI2C_H
#define __MYI2C_H

#include <stm32f4xx.h>

void MY_I2C_Init(void);
void MY_I2C_Send(uint8_t Address, uint32_t data, uint8_t size);
void MY_I2C_Receive(uint8_t Address, uint8_t Register, uint8_t *ReceiveBuff, uint8_t size);
void MY_I2C_Start(uint8_t addr);
void I2C_Addr_Detect(void);

#endif
