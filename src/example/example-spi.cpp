//#include "stm32lib-conf.h"
#include "gpio.h"
#include "scheduling.h"
#include "spi.h"
Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };

void initGpio()
{
    Gpio gpioNss = { GPIOB, 12 };
    Gpio gpioMosi = { GPIOB, 15 };
    Gpio gpioMiso = { GPIOB, 14 };
    Gpio gpioSck = { GPIOB, 13 };

    gpioNss.init(GPIO_Mode_AF_PP);
    gpioMosi.init(GPIO_Mode_AF_PP);
    gpioMiso.init(GPIO_Mode_AF_PP);
    gpioSck.init(GPIO_Mode_AF_PP);
    
}
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

    initGpio();
    Spi spi = Spi();
    spi.init();
    spi.enable(ENABLE);
    while (1) {
        spi.write("a", 1);
        gpioLed.toggle();
        mDelay(500);
    }

    return 0;
}
