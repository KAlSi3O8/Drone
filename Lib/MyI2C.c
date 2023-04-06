#include "MyI2C.h"
#include <stdio.h>

//I2C Config
void MY_I2C_Init(void){
    //GPIO Define
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER |= (0x2 << GPIO_MODER_MODER8_Pos) | (0x2 << GPIO_MODER_MODER9_Pos);
    GPIOB->OTYPER |= GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9;
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9;
    GPIOB->PUPDR |= (0x1 << GPIO_PUPDR_PUPD8_Pos) | (0x1 << GPIO_PUPDR_PUPD9_Pos);
    GPIOB->AFR[1] |= (0x4 << GPIO_AFRH_AFSEL8_Pos) | (0x4 << GPIO_AFRH_AFSEL9_Pos);
    
    //I2C Setting
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    I2C1->CR2 &= ~I2C_CR2_FREQ;
    I2C1->CR2 |= 42;
    I2C1->CR1 &= ~I2C_CR1_PE;
    I2C1->TRISE = 13;
    I2C1->CCR = I2C_CCR_FS | 70;
    I2C1->CR1 |= I2C_CR1_PE;
    I2C1->CR1 |= I2C_CR1_ACK;
}

//I2C Send Byte(s)
void MY_I2C_Send(uint8_t Address, uint32_t data, uint8_t size){
    uint8_t *p = (uint8_t *)data;
    
    //Wait for Bus Idle
    while(I2C1->SR2 & I2C_SR2_BUSY);
    //Start Transmit
    I2C1->CR1 |= I2C_CR1_START;
    while(!(I2C1->SR1 & I2C_SR1_SB));
    I2C1->DR = Address & ~1;
    while(!(I2C1->SR1 & I2C_SR1_ADDR));
    I2C1->SR2;
    
    //Send One Byte
    if(size == 1){
        while(!(I2C1->SR1 & I2C_SR1_TXE));
        I2C1->DR = data & 0xFF;
        size = 0;
    }
    //Send Several Bytes
    while(size > 0){
        while(!(I2C1->SR1 & I2C_SR1_TXE));
        I2C1->DR = *p;
        p++;
        size--;
    }
    while(!(I2C1->SR1 & I2C_SR1_TXE));
    
    //Stop Transmit
    I2C1->CR1 |= I2C_CR1_STOP;
}

//I2C Revceive Byte(s)
void MY_I2C_Receive(uint8_t Address, uint8_t Register, uint8_t *ReceiveBuff, uint8_t size){
    //Wait for Bus Idle
    while(I2C1->SR2 & I2C_SR2_BUSY);
    //Write Register
    I2C1->CR1 |= I2C_CR1_START;
    while(!(I2C1->SR1 & I2C_SR1_SB));
    I2C1->DR = Address & ~1;
    while(!(I2C1->SR1 & I2C_SR1_ADDR));
    I2C1->SR2;
    I2C1->DR = Register;
    while(!(I2C1->SR1 & I2C_SR1_TXE));
    
    
    //Read Data
    I2C1->CR1 |= I2C_CR1_START;
    while(!(I2C1->SR1 & I2C_SR1_SB));
    I2C1->DR = Address | 1;
    while(!(I2C1->SR1 & I2C_SR1_ADDR));
    I2C1->SR2;
    I2C1->CR1 |= I2C_CR1_ACK;
    while(size > 3){
        while(!(I2C1->SR1 & I2C_SR1_RXNE));
        *ReceiveBuff = I2C1->DR;
        ReceiveBuff++;
        size--;
    }
    while(!(I2C1->SR1 & I2C_SR1_BTF));
    I2C1->CR1 &= ~I2C_CR1_ACK;
    *ReceiveBuff = I2C1->DR;
    ReceiveBuff++;
    while(!(I2C1->SR1 & I2C_SR1_BTF));
    I2C1->CR1 |= I2C_CR1_STOP;
    *ReceiveBuff = I2C1->DR;
    ReceiveBuff++;
    *ReceiveBuff = I2C1->DR;
}

//I2C Start Transmit
void MY_I2C_Start(uint8_t addr){
    while(I2C1->SR2 & I2C_SR2_BUSY);
    I2C1->CR1 |= I2C_CR1_START;
    while(!(I2C1->SR1 & I2C_SR1_SB));
    I2C1->DR = addr;
    while(!(I2C1->SR1 & I2C_SR1_ADDR));
    I2C1->SR2;
}

//I2C Address Detective
void I2C_Addr_Detect(void){
    uint8_t Addr = 0;
    uint32_t cnt = 0;
    //Wait for Bus Idle
    while(I2C1->SR2 & I2C_SR2_BUSY);
    //Start Transmit
    while(Addr < 255){
        I2C1->CR1 |= I2C_CR1_START;
        while(!(I2C1->SR1 & I2C_SR1_SB));
        I2C1->DR = Addr & ~1;
        cnt = 84000;
        while(!(I2C1->SR1 & I2C_SR1_ADDR) && cnt){
            cnt--;
        }
        if(cnt == 0){
            Addr++;
            I2C1->CR1 |= I2C_CR1_STOP;
        }
        else{
            I2C1->SR2;
            printf("%x\r\n", Addr);
            I2C1->CR1 |= I2C_CR1_STOP;
            Addr++;
        }
    }
    printf("END\r\n\r\n");
}
