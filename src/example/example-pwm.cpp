#include "stm32lib-conf.h"

Timer& timer = GPIO_LED1_TIMER;
Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };
TimerChannelOutput tco { &timer, GPIO_LED1_TIM_CH };

int main()
{
    configureClocks(1000);
    gpioLed.init(GPIO_LED1_MODE);

#if defined(STM32F1)
    gpioLed.configRemap(GPIO_LED1_REMAP, ENABLE);
#elif defined(STM32F0) || defined(STM32F3)
    gpioLed.configAF(GPIO_LED1_AF);
#else
 #error
#endif

    timer.initFreq(10e4); // 10kHz pwm frequency
    timer.setEnabled(ENABLE);
    timer.setMOE(ENABLE);

    tco.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable, TIM_OutputNState_Enable);

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
