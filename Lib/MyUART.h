#ifndef __MyUART_H
#define __MyUART_H

#include <stm32f4xx.h>
#define FILE void
#define ReceiveBuffSize 128

extern uint8_t ReceiveBuff[ReceiveBuffSize];

void MY_UART_Init(void);
void MY_UART_Send(uint8_t *string, uint8_t size);


#endif
