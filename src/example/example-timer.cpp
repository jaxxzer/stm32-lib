#include "stm32lib-conf.h"

#if  defined(STM32F0)
    Gpio gpio_Led { GPIOB, 1 };
#elif defined(STM32F1)
    Gpio gpio_Led { GPIOC, 13 };
#elif defined(STM32F3)
    Gpio gpio_Led { GPIOC, 9 };
#else
#error
#endif

// Callback to run on UPDATE interrupt event
void tim1UpdateCallback(void)
{
    gpio_Led.toggle();
}

int main()
{
    configureClocks(1000);

#if defined(STM32F0)
    gpio_Led.init(GPIO_Mode_OUT);
    nvic_config(TIM1_BRK_UP_TRG_COM_IRQn, 0, 0, ENABLE);
#elif defined(STM32F1)
    gpio_Led.init(GPIO_Mode_Out_PP);
    nvic_config(TIM1_UP_IRQn, 0, 0, ENABLE);
#elif defined(STM32F3)
    gpio_Led.init(GPIO_Mode_OUT);
    nvic_config(TIM1_UP_TIM16_IRQn, 0, 0, ENABLE);
#endif

    timer1.initFreq(10); // 10Hz update
    timer1.setupUpCallback(&tim1UpdateCallback);
    timer1.setEnabled(ENABLE);
    timer1.interruptConfig(TIM_IT_Update, ENABLE);

    while (1) {  }

    return 0;
}
