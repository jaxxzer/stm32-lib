#include "stm32lib-conf.h"

Timer& timer = GPIO_LED1_TIMER;
Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };
TimerChannelOutput tco { &timer, GPIO_LED1_TIM_CH };

uint16_t pulses[] = { 255, 0xfff, 0xfff, 255, 255, 0xffff, 0, 0, 0, 0xffff, 0, 0, 0, 0xf, 0xf};
int main()
{
    configureClocks(1000);
    gpioLed.init(GPIO_Mode_AF);
#if defined(STM32F1)
    gpioLed.configRemap(GPIO_LED1_REMAP, ENABLE);
#elif defined(STM32F0) || defined(STM32F3)
    gpioLed.configAF(GPIO_LED1_AF);
#else
 #error
#endif
    nvic_config(TIM2_IRQn, 0, 0, ENABLE);

    timer.initFreq(4); // 1Hz update frequency
    tco.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable);


    // Using channel 2, ADC (when declared) uses channel 1
    Dma dma1c2 = Dma(DMA1_Channel2);

    dma1c2.init((uint32_t)&(TIM2->CCR1),
                (uint32_t)&pulses[0],
                sizeof(pulses)/2,
                DMA_DIR_PeripheralDST,
                DMA_PeripheralDataSize_Word,
                DMA_MemoryDataSize_HalfWord,
                DMA_Mode_Normal,
                DMA_Priority_Medium,
                DMA_MemoryInc_Enable,
                DMA_PeripheralInc_Disable,
                DMA_M2M_Disable);

    dma1c2.setEnabled(ENABLE);
    //TIM1->DCR = TIM_DMABase_CCR1 | TIM_DMABurstLength_1Transfer;
    TIM_DMACmd(timer.peripheral(), TIM_DMA_Update, ENABLE);
//TIM_SelectCCDMA(timer.peripheral(), ENABLE);
        timer.interruptConfig(TIM_IT_Update, ENABLE);

    timer.setEnabled(ENABLE);

    timer.setMOE(ENABLE);
    while (1) { 
        mDelay(5000);
            dma1c2.setEnabled(DISABLE);

        DMA1_Channel2->CNDTR = sizeof(pulses)/2;
	//DMA_ClearFlag(DMA1_FLAG_TC2);
                dma1c2.setEnabled(ENABLE);

    }

    return 0;
}
