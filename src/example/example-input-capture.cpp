#include "stm32lib-conf.h"

Timer& timer = GPIO_LED1_TIMER;
Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };
TimerChannelOutput tco { &timer, GPIO_LED1_TIM_CH };


Timer& timerCapture = CAPTURE_TIMER;
Gpio gpioCapture { GPIO_CAPTURE_PORT, GPIO_CAPTURE_PIN };
TimerChannelInput tciRising { &timerCapture, GPIO_CAPTURE_TIM_CH_RISING };
TimerChannelOutput tcoFalling { &timerCapture, GPIO_CAPTURE_TIM_CH_FALLING };

uint16_t riseCapture;
uint32_t lastCapture;
void risingCallback(void)
{
    lastCapture = microseconds;
    riseCapture = tciRising._peripheral->CCR1;
}


void fallingCallback(void)
{
    lastCapture = microseconds;
    riseCapture = tciRising._peripheral->CCR2;
}


int main()
{
    configureClocks(1000);

#if defined(STM32F1)
    gpioLed.init(GPIO_Mode_AF_PP);
    gpioCapture.init(GPIO_Mode_AF_OD);
    gpioLed.configRemap(GPIO_LED1_REMAP, ENABLE);
    gpioLed.configRemap(GPIO_CAPTURE_REMAP, ENABLE);
#elif defined(STM32F0) || defined(STM32F3)
    gpioLed.init(GPIO_Mode_AF);
    gpioCapture.init(GPIO_Mode_AF);
    gpioLed.configAF(GPIO_LED1_AF);
    gpioCapture.configAF(GPIO_CAPTURE_AF);
#else
 #error
#endif

    timer.initFreq(1e3); // 10kHz pwm frequency
    timer.setEnabled(ENABLE);
    timer.setMOE(ENABLE);

    tco.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable, TIM_OutputNState_Enable);

    timerCapture.setupCc1Callback(&risingCallback);
    timerCapture.setupCc2Callback(&fallingCallback);
    timerCapture.initFreq(1e3); // 1MHz capture frequency
    timerCapture.setEnabled(ENABLE);    
    timerCapture.interruptConfig(TIM_IT_CC1, ENABLE);
    timerCapture.interruptConfig(TIM_IT_CC2, ENABLE);

    // Note CCxS bits only writable when CCxE is 0 (channel is disabled)
    tciRising.init(TIM_ICPolarity_Rising, 0xF);
    tcoFalling.init(TIM_ICPolarity_Falling, 0xF, TIM_ICPSC_DIV1, TIM_ICSelection_IndirectTI);



#if defined(STM32F0)
    nvic_config(TIM1_BRK_UP_TRG_COM_IRQn, 0, ENABLE);
#elif defined(STM32F1)
    nvic_config(TIM1_UP_IRQn, 0, 0, ENABLE);
#elif defined(STM32F3)
    nvic_config(TIM1_CC_IRQn, 0, 0, ENABLE);
#endif

    // breath
    uint16_t duty = 0;
    int8_t inc = 75;
    while (1) { 
        mDelay(1);
        tco.setDuty(duty);
        if ( (inc > 0 && inc > 65535 - duty) ||
             (inc < 0 && duty < -inc) )
        {
            inc = -inc;
        }       
        duty += inc;
     }

    return 0;
}
