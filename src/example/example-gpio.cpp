//#include "stm32lib-conf.h"
#include "gpio.h"
#include "scheduling.h"
Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };

int main(void)
{
	configureClocks(1000);

#if defined(STM32F0) || defined(STM32F3)
    gpioLed.init(GPIO_Mode_OUT);
#elif defined(STM32F1)
    gpioLed.init(GPIO_Mode_Out_PP);
#else
 #error
#endif

    while (1) {
        gpioLed.toggle();
        mDelay(500);
    }

    return 0;
}
