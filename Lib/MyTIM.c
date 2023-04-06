#include "MyTIM.h"


int16_t Receiver_Data[9] = {0};
uint8_t CH = 0;

//TIM Config
void MY_TIM_Init(void){
    //GPIO Define
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    
    GPIOA->MODER |= (0x2 << GPIO_MODER_MODER8_Pos) | (0x2 << GPIO_MODER_MODER6_Pos) | (0x2 << GPIO_MODER_MODER7_Pos);
    GPIOA->AFR[1] |= (0x1 << GPIO_AFRH_AFSEL8_Pos);
    GPIOB->MODER |= (0x2 << GPIO_MODER_MODER0_Pos) | (0x2 << GPIO_MODER_MODER1_Pos);
    GPIOA->AFR[0] |= (0x2 << GPIO_AFRL_AFSEL6_Pos) | (0x2 << GPIO_AFRL_AFSEL7_Pos);
    GPIOB->AFR[0] |= (0x2 << GPIO_AFRL_AFSEL0_Pos) | (0x2 << GPIO_AFRL_AFSEL1_Pos);
    
    //TIM1CH1 Input Capture
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    TIM1->CCMR1 |= 0x1 << TIM_CCMR1_CC1S_Pos;
    TIM1->PSC = 83;
    #if USE_IT
    TIM1->DIER |= TIM_DIER_UIE | TIM_DIER_CC1IE;
    TIM1->ARR = 3000;
    #else
    TIM1->ARR = 19999;
    #endif
    TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1P;
    TIM1->CR1 |= TIM_CR1_CEN;
    
    #if USE_IT
    NVIC->IP[((uint32_t)TIM1_CC_IRQn)] = (uint8_t)((1<<1 | 0) << 4);
    NVIC->IP[((uint32_t)TIM1_UP_TIM10_IRQn)] = (uint8_t)((1<<1 | 1) << 4);
    NVIC->ISER[(((uint32_t)TIM1_CC_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)TIM1_CC_IRQn) & 0x1FUL));
    NVIC->ISER[(((uint32_t)TIM1_UP_TIM10_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)TIM1_UP_TIM10_IRQn) & 0x1FUL));
    #endif
    
    //TIM3CH1\2\3\4 Output PWM
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->CCMR1 |= TIM_CCMR1_OC1FE | (0x6 << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC2FE | (0x6 << TIM_CCMR1_OC2M_Pos);
    TIM3->CCMR2 |= TIM_CCMR2_OC3FE | (0x6 << TIM_CCMR2_OC3M_Pos) | TIM_CCMR2_OC4FE | (0x6 << TIM_CCMR2_OC4M_Pos);
    TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
    TIM3->PSC = 83;
    TIM3->ARR = 5000;
    TIM3->CCR1 = 0;
    TIM3->CCR2 = 0;
    TIM3->CCR3 = 0;
    TIM3->CCR4 = 0;
    TIM3->CR1 |= TIM_CR1_CEN;
    
    MY_Delay_Init();
    MY_TIMER_Init();
}

#if !USE_IT
//TIM Read CCR
void MY_Read_CCR(void){
    if(TIM1->SR & TIM_SR_CC1IF){
        TIM1->SR &= ~TIM_SR_CC1IF;
        if(TIM1->CCR1 > 2500){
            CH = 0;
        }
        Receiver_Data[CH++] = TIM1->CCR1;
        TIM1->CNT = 0;
    }
}
#endif

//TIM Config for Delay
void MY_Delay_Init(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    TIM4->PSC = 83;
    TIM4->ARR = 1000;
    TIM4->EGR |= TIM_EGR_UG;
    TIM4->SR &= ~TIM_SR_UIF;
}

//TIM Config for Counting Time
void MY_TIMER_Init(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM5EN;
    TIM2->PSC = 84;
    TIM2->ARR = 20000;
    TIM2->CNT = 0;
    /*
    TIM5->PSC = 9;
    TIM5->EGR |= TIM_EGR_UG;
    TIM5->SR &= ~TIM_SR_UIF;
    TIM5->CNT = 0;
    */
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC->IP[(uint32_t)TIM2_IRQn] |= (uint8_t)((2<<1 | 0) << 4);
    NVIC->ISER[(((uint32_t)TIM2_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)TIM2_IRQn) & 0x1FUL));
    
}

//My Delay func
void MY_Delay(uint32_t time){
    TIM4->CNT = 0;
    TIM4->CR1 = TIM_CR1_CEN;
    while(time > 0){
        if(TIM4->SR & TIM_SR_UIF){
            time--;
            TIM4->SR &= ~TIM_SR_UIF;
        }
    }
}

void TIM1_CC_IRQHandler(void){
    Receiver_Data[CH++] = TIM1->CCR1;
    TIM1->CNT = 0;
}

void TIM1_UP_TIM10_IRQHandler(void){
    CH = 0;
    TIM1->SR &= ~TIM_SR_UIF;
}

void TIM2_IRQHandler(void){
    flag = 1;
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->CNT = 0;
    TIM2->SR &= ~TIM_SR_UIF;
}
