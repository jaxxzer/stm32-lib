#include "stm32lib-conf.h"

Gpio gpioLed{GPIO_LED1_PORT, GPIO_LED1_PIN};

int main() {
  configureClocks(1000);

#if defined(STM32F0) || defined(STM32F3)
  gpioLed.init(GPIO_Mode_OUT);
#elif defined(STM32F1)
  gpioLed.init(GPIO_Mode_Out_PP);
#else
#error
#endif

  while (1) {
    // fprintf((FILE*)FD_ITM_CH0, "hello");
    ITM_SendChar('a');
    ITM_SendChar('b');
    ITM_SendChar('c');
    ITM_SendChar('\r');
    ITM_SendChar('\n');

    mDelay(100);
    gpioLed.toggle();
  }

  return 0;
}
