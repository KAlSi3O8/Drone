#include "MyUART.h"

uint8_t ReceiveBuff[ReceiveBuffSize] = {0};

//UART Config
void MY_UART_Init(void){
    //GPIO Define
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER |= GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1;
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7;
    GPIOB->AFR[0] |= (0x7 << GPIO_AFRL_AFSEL6_Pos) | (0x7 << GPIO_AFRL_AFSEL7_Pos);
    
    //UART Setting
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART1->BRR = 0x2D9;
    USART1->CR1 |= USART_CR1_UE;
    
    USART1->CR1 |= USART_CR1_RXNEIE | USART_CR1_IDLEIE;
    NVIC->IP[((uint32_t)USART1_IRQn)] = (uint8_t)((2<<1 | 0) << 4);
    NVIC->ISER[(((uint32_t)USART1_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)USART1_IRQn) & 0x1FUL));
}

//UART Send Data
void MY_UART_Send(uint8_t *string, uint8_t size){
    while(size){
        while(!(USART1->SR & USART_SR_TXE));
        USART1->DR = *string;
        string++;
        size--;
    }
}

int fputc(int c, FILE *f){
	while(!(USART1->SR & USART_SR_TXE));
	USART1->DR = (c & 0xFF);
	
	return c;
}

//UART Interupt Handler
void USART1_IRQHandler(void){
    static uint32_t RL = 0;
    if(USART1->SR & USART_SR_RXNE && RL < ReceiveBuffSize-1){
        ReceiveBuff[RL] = USART1->DR;
        RL++;
    }
    if(USART1->SR & USART_SR_IDLE){
        ReceiveBuff[RL] = 0;
        RL = 0;
        USART1->DR;
    }
}
