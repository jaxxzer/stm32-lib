#include "stm32lib-conf.h"

GPIO_TypeDef* ports[] = { GPIOA, GPIOB, GPIOC, GPIOD, GPIOE };


void testPin(uint8_t port, uint8_t pin)
{
    Gpio gpio { ports[port], pin };

    if  ((port == GPIOA_BASE && pin==13) ||
        (port == GPIOA_BASE && pin==14))
    {
        return;
    }


#if defined(STM32F0) || defined(STM32F3)
    gpio.init(GPIO_Mode_OUT);
#elif defined(STM32F1)
    gpio.init(GPIO_Mode_Out_PP);
#else
 #error
#endif

    for (int i = 0; i < port; i++) {
        gpio.reset();
        mDelay(1);
        gpio.set();
        mDelay(1);
        gpio.reset();
    }

    mDelay(5);

    for (int i = 0; i < pin; i++) {
        gpio.reset();
        mDelay(1);
        gpio.set();
        mDelay(1);
        gpio.reset();
    }
}

int main(void)
{
	configureClocks(1000);


    while (1) {
        for (int i = 0; i < 5; i++) {
            for (int j = 0; i < 16; i++) {
                //testPin(i, j);
            }
        }  
    }

    return 0;
}


