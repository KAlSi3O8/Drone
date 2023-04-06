#include "main.h"
//#define USE_EXTERNAL_CLOCK
#define SHOW_EULRA 1

uint8_t flag;

int main(void){
    MY_System_Config();
    MY_TIM_Init();
    MY_I2C_Init();
    OLED_Init();
    Motor_Init();
    MY_UART_Init();
    MY_Delay(100);
    ESP_Init();
    MPU_Init();
    HMC_Init();
    
    OLED_ResetOffset();
    OLED_ClnAllDRAM();
    OLED_All(0);
    //Show_Name();
    GY86_adj();
    TIM2->CR1 |= TIM_CR1_CEN;
    
    while(1){
        if(flag){
            MPU_Read();
            MPU_Trans();
            HMC_Read();
            HMC_Trans();
            My_Madgwick_Update(Acc_Data.X, Acc_Data.Y, Acc_Data.Z, Gyro_Data.X, Gyro_Data.Y, Gyro_Data.Z, Mag_Data.X, Mag_Data.Y, Mag_Data.Z);
            Anotc_Send_Eulra(eulra.roll, eulra.pitch, eulra.yaw);
//            Anotc_Send_Raw(MPU_Data.AX, MPU_Data.AY, MPU_Data.AZ, MPU_Data.GX, MPU_Data.GY, MPU_Data.GZ, HMC_Data.X, HMC_Data.Y, HMC_Data.Z);
//            Anotc_Send_PID();
            //Show_Data();
            flag = 0;
            TIM2->CR1 |= TIM_CR1_CEN;
        }
        else{
            #if !USE_IT
            MY_Read_CCR();
            #endif
            Motor_Output();
        }
    }
}

void MY_System_Config(void){
    //Clock Reset
    RCC->CR |= RCC_CR_HSION;
    RCC->CFGR = 0x00000000;
    RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_CSSON | RCC_CR_PLLON);
    RCC->PLLCFGR = RCC_PLLCFGR_RST_VALUE;
    RCC->CR &= ~RCC_CR_HSEBYP;
    RCC->CIR = 0x00000000;
    
    //Enable HSE Set PLL as System Clock
    #ifdef USE_EXTERNAL_CLOCK
    RCC->CR |= RCC_CR_HSEBYP;
    #endif
    RCC->CR |= RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY));
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
                   (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_2WS;
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
    
    //Update SystemCoreClock
    SystemCoreClock = 84000000;
    NVIC_SetPriorityGrouping(4);
    
    SysTick->VAL = 0L;
    SysTick->LOAD = 84000;
    SysTick->CTRL = 0x00;
}


//Show basic DRAW
void Show_Name(void){
    OLED_SS(0, 0, (uint8_t *)"AX:", (uint32_t)&Atali, 1);
    OLED_SS(0, 8, (uint8_t *)"AY:", (uint32_t)&Atali, 1);
    OLED_SS(0, 16, (uint8_t *)"AZ:", (uint32_t)&Atali, 1);
    OLED_SS(0, 24, (uint8_t *)"GX:", (uint32_t)&Atali, 1);
    OLED_SS(0, 32, (uint8_t *)"GY:", (uint32_t)&Atali, 1);
    OLED_SS(0, 40, (uint8_t *)"GZ:", (uint32_t)&Atali, 1);
    OLED_SS(0, 48, (uint8_t *)"T:", (uint32_t)&Atali, 1);
    OLED_SS(72, 0, (uint8_t *)"X:", (uint32_t)&Atali, 1);
    OLED_SS(72, 8, (uint8_t *)"Y:", (uint32_t)&Atali, 1);
    OLED_SS(72, 16, (uint8_t *)"Z:", (uint32_t)&Atali, 1);
    #if SHOW_EULRA
    OLED_SS(72, 32, (uint8_t *)"R:", (uint32_t)&Atali, 1);
    OLED_SS(72, 40, (uint8_t *)"P:", (uint32_t)&Atali, 1);
    OLED_SS(72, 48, (uint8_t *)"Y:", (uint32_t)&Atali, 1);
    #else
    OLED_DT(64, 25, 61, 31, 1);
    OLED_DT(65, 26, 29, 29, 0);
    OLED_DT(95, 26, 29, 29, 0);
    OLED_WriteRAM(64, 25, 61, 31);
    #endif
}
//Show GY-86 Data
void Show_Data(void){
    #if !SHOW_EULRA
    uint16_t ch1, ch2, ch3, ch4;
    #endif
    OLED_ShowNum(20, 0, MPU_Data.AX, (uint32_t)&Atali, 1, 6);
    OLED_ShowNum(20, 8, MPU_Data.AY, (uint32_t)&Atali, 1, 6);
    OLED_ShowNum(20, 16, MPU_Data.AZ, (uint32_t)&Atali, 1, 6);
    OLED_ShowNum(20, 24, MPU_Data.GX, (uint32_t)&Atali, 1, 5);
    OLED_ShowNum(20, 32, MPU_Data.GY, (uint32_t)&Atali, 1, 5);
    OLED_ShowNum(20, 40, MPU_Data.GZ, (uint32_t)&Atali, 1, 5);
    OLED_ShowNum(12, 48, MPU_Data.Temp, (uint32_t)&Atali, 1, 4);
    OLED_ShowNum(84, 0, HMC_Data.X, (uint32_t)&Atali, 1, 5);
    OLED_ShowNum(84, 8, HMC_Data.Y, (uint32_t)&Atali, 1, 5);
    OLED_ShowNum(84, 16, HMC_Data.Z, (uint32_t)&Atali, 1, 5);
    #if SHOW_EULRA
    OLED_ShowNum(84, 32, (int)eulra.roll,  (uint32_t)&Atali, 1, 5);
    OLED_ShowNum(84, 40, (int)eulra.pitch, (uint32_t)&Atali, 1, 5);
    OLED_ShowNum(84, 48, (int)eulra.yaw,   (uint32_t)&Atali, 1, 5);
    #else
    OLED_DT(65, 26, 29, 29, 0);
    OLED_DT(95, 26, 29, 29, 0);
    ch1 = Receiver_Data[1]>1000?((Receiver_Data[1]-1000)*29/1000):0;
    ch2 = Receiver_Data[2]>1000?((Receiver_Data[2]-1000)*29/1000):0;
    ch3 = Receiver_Data[3]>1000?((Receiver_Data[3]-1000)*29/1000):0;
    ch4 = Receiver_Data[4]>1000?((Receiver_Data[4]-1000)*29/1000):0;
    OLED_Point(95+ch1, 54-ch2, 1);
    OLED_Point(65+ch4, 54-ch3, 1);
    OLED_WriteRAM(64, 25, 61, 31);
    #endif
}

//Send Data Through Printf
void Send_Data(void){
    printf("MPU\r\nAccx: %02f\tAccy: %02f\tAccz: %02f\r\nTEMP: %d\r\nGyrox: %02f\tGyroy: %02f\tGyroz: %02f\r\n\r\n", \
    Acc_Data.X, Acc_Data.Y, Acc_Data.Z, MPU_Data.Temp, Gyro_Data.X, Gyro_Data.Y, Gyro_Data.Z);
    printf("HMC\r\nX: %d\tY: %d\tZ: %d\r\n\r\n", \
    HMC_Data.X, HMC_Data.Y, HMC_Data.Z);
    printf("Receiver\r\nCH1: %d\tCH2: %d\tCH3: %d\tCH4: %d\r\nCH5: %d\tCH6: %d\tCH7: %d\tCH8: %d\r\n\r\n", \
    Receiver_Data[1], Receiver_Data[2], Receiver_Data[3], Receiver_Data[4], Receiver_Data[5], Receiver_Data[6], Receiver_Data[7], Receiver_Data[8]);
}
