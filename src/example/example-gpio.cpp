#include "stm32lib-conf.h"

Gpio gpio_Led { GPIO_LED1_PORT, GPIO_LED1_PIN };

static const int delay_ms = 50;

int main(void)
{
	configureClocks(1000);

    gpio_Led.init(GPIO_LED1_MODE);

    while (1) {
        gpio_Led.toggle();
        mDelay(delay_ms);
    }

    return 0;
}
