#include "gpio.h"
#include "scheduling.h"
GPIO_TypeDef* ports[] = { GPIOA, GPIOB, GPIOC, GPIOD, GPIOE };


void testPin(uint8_t port, uint8_t pin)
{

    if  (
        ((uint32_t)ports[port] == GPIOA_BASE && pin == 13) ||
        ((uint32_t)ports[port] == GPIOA_BASE && pin == 14) ||
        ((uint32_t)ports[port] == GPIOB_BASE && pin == 1)  ||
        ((uint32_t)ports[port] == GPIOA_BASE && pin == 0)        )
    {
        return;
    }

    Gpio gpio { ports[port], pin };

#if defined(STM32F0) || defined(STM32F3)
    //gpio.init(GPIO_Mode_OUT);
#elif defined(STM32F1)
    gpio.init(GPIO_Mode_IN_FLOATING);
    return;
#else
 #error
#endif
}

int main(void)
{
	configureClocks(1000);
    Gpio gpioLed { GPIOA, 0 };
    gpioLed.init(GPIO_Mode_Out_PP);
    gpioLed.reset();
    for (uint8_t i = 0; i < 5; i++) {
        for (uint8_t j = 0; j < 16; j++) {
            testPin(i, j);
        }
    }
    while(1);
    return 0;
}
