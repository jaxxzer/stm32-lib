#include "stm32lib-conf.h"

Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };

// Callback to run on UPDATE interrupt event
void tim1UpdateCallback(void)
{
    gpioLed.toggle();
}

int main()
{
    configureClocks(1000);

#if defined(STM32F0) || defined(STM32F3)
    gpioLed.init(GPIO_Mode_OUT);
#elif defined(STM32F1)
    gpioLed.init(GPIO_Mode_Out_PP);
#else
 #error
#endif

#if defined(STM32F0)
    nvic_config(TIM1_BRK_UP_TRG_COM_IRQn, 0, ENABLE);
#elif defined(STM32F1)
    nvic_config(TIM1_UP_IRQn, 0, 0, ENABLE);
#elif defined(STM32F3)
    nvic_config(TIM1_UP_TIM16_IRQn, 0, 0, ENABLE);
#endif

    timer1.initFreq(10); // 10Hz update
    timer1.setupUpCallback(&tim1UpdateCallback);
    timer1.setEnabled(ENABLE);
    timer1.interruptConfig(TIM_IT_Update, ENABLE);

    while (1) {  }

    return 0;
}
