#include "stm32lib-conf.h"

Timer& timer = GPIO_LED1_TIMER;
Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };
TimerChannelOutput tco { &timer, GPIO_LED1_TIM_CH };

static const uint16_t dshot_0 = 600;
static const uint16_t dshot_1 = 2 * dshot_0;
static const uint16_t dshot_period = 3 * dshot_0;
uint16_t pulses[] = { dshot_1, dshot_1, dshot_1, dshot_1, dshot_1, dshot_1, dshot_1, dshot_1, dshot_1, dshot_1, dshot_1, dshot_0, dshot_1, dshot_1, dshot_1, dshot_0, 0 };

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
    //nvic_config(TIM2_IRQn, 0, 0, ENABLE);

    //timer.initFreq(10000);
    timer.init(0, dshot_period);
    timer.setCCPreloadControl(ENABLE);
    tco.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable);
    tco.preloadConfig(ENABLE);


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
        //timer.interruptConfig(TIM_IT_Update, ENABLE);

    timer.setEnabled(ENABLE);

    timer.setMOE(ENABLE);
    while (1) { 
        mDelay(100);
            dma1c2.setEnabled(DISABLE);

        DMA1_Channel2->CNDTR = sizeof(pulses)/2;
	//DMA_ClearFlag(DMA1_FLAG_TC2);
                dma1c2.setEnabled(ENABLE);
                timer.generateEvent(TIM_EventSource_Update);

    }

    return 0;
}
