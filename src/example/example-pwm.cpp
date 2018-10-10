#include "stm32lib-conf.h"

#ifdef STM32F0
    Timer& timer = timer3;
    Gpio gpio_Led { GPIOB, 1 };
    TimerChannelOutput tco { TIM3, TIM_Channel_4 };
#elif STM32F1
    Timer& timer = timer1;
    Gpio gpio_Led { GPIOB, 13 };
    TimerChannelOutput tco { TIM1, TIM_Channel_1 };
#endif

int main()
{
    configureClocks(1000);

#ifdef STM32F0
    gpio_Led.init(GPIO_Mode_AF);
    gpio_Led.configAF(1);
#elif STM32F1
    gpio_Led.init(GPIO_Mode_AF_PP);
#endif

    timer.setClockEnabled(ENABLE);
    timer.initFreq(1e4); // 10kHz pwm frequency
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
