#include "stm32lib-conf.h"
#define USART_BAUDRATE 1e6

Gpio gpioLed{GPIO_LED1_PORT, GPIO_LED1_PIN};

#if defined(USE_USART_1)
Gpio gpioUsart1Tx{GPIO_USART1_TX_PORT, GPIO_USART1_TX_PIN};
Gpio gpioUsart1Rx{GPIO_USART1_RX_PORT, GPIO_USART1_RX_PIN};
#endif

#if defined(USE_USART_2)
Gpio gpioUsart2Tx{GPIO_USART2_TX_PORT, GPIO_USART2_TX_PIN};
Gpio gpioUsart2Rx{GPIO_USART2_RX_PORT, GPIO_USART2_RX_PIN};
#endif

#if defined(USE_USART_3)
Gpio gpioUsart3Tx{GPIO_USART3_TX_PORT, GPIO_USART3_TX_PIN};
Gpio gpioUsart3Rx{GPIO_USART3_RX_PORT, GPIO_USART3_RX_PIN};
#endif

#if defined(USE_USART_1)
void initUsart1(void) {
#if defined(STM32F0)
  nvic_config(USART1_IRQn, 0, ENABLE);
#elif defined(STM32F1) || defined(STM32F3)
  nvic_config(USART1_IRQn, 0, 0, ENABLE);
#else
#error
#endif

  gpioUsart1Rx.init(GPIO_Mode_AF, GPIO_PuPd_UP);
  gpioUsart1Tx.init(GPIO_Mode_AF, GPIO_PuPd_UP);

#if defined(STM32F0) || defined(STM32F3)
  gpioUsart1Rx.configAF(GPIO_USART1_RX_AF);
  gpioUsart1Tx.configAF(GPIO_USART1_TX_AF);
#elif defined(STM32F1)
  Gpio::remapConfig(GPIO_USART1_REMAP, ENABLE);
#else
#error
#endif

  uart1.init(USART_BAUDRATE);
  uart1.ITConfig(USART_IT_RXNE, ENABLE);
  uart1.setEnabled(ENABLE);
  uart1.cls();
}
#endif

#if defined(USE_USART_2)
void initUsart2(void) {
#if defined(STM32F0)
  nvic_config(USART2_IRQn, 0, ENABLE);
#elif defined(STM32F1) || defined(STM32F3)
  nvic_config(USART2_IRQn, 0, 0, ENABLE);
#else
#error
#endif

  gpioUsart2Rx.init(GPIO_Mode_AF);
  gpioUsart2Tx.init(GPIO_Mode_AF);

#if defined(STM32F0) || defined(STM32F3)
  gpioUsart2Rx.configAF(GPIO_USART2_RX_AF);
  gpioUsart2Tx.configAF(GPIO_USART2_TX_AF);
#elif defined(STM32F1)
  Gpio::remapConfig(GPIO_USART2_REMAP, ENABLE);
#else
#error
#endif

  uart2.init(USART_BAUDRATE);
  uart2.ITConfig(USART_IT_RXNE, ENABLE);
  uart2.setEnabled(ENABLE);
  uart2.cls();
}
#endif

#if defined(USE_USART_3)
void initUsart3(void) {
#if defined(STM32F0)
  nvic_config(USART3_IRQn, 0, ENABLE);
#elif defined(STM32F1) || defined(STM32F3)
  nvic_config(USART3_IRQn, 0, 0, ENABLE);
#else
#error
#endif

  gpioUsart3Rx.init(GPIO_Mode_AF);
  gpioUsart3Tx.init(GPIO_Mode_AF);

#if defined(STM32F0) || defined(STM32F3)
  gpioUsart3Rx.configAF(GPIO_USART3_RX_AF);
  gpioUsart3Tx.configAF(GPIO_USART3_TX_AF);
#elif defined(STM32F1)
  Gpio::remapConfig(GPIO_USART3_REMAP, ENABLE);
#else
#error
#endif

  uart3.init(USART_BAUDRATE);
  uart3.ITConfig(USART_IT_RXNE, ENABLE);
  uart3.setEnabled(ENABLE);
  uart3.cls();
}
#endif

void initGpioLed(void) {
#if defined(STM32F0) || defined(STM32F3)
  gpioLed.init(GPIO_Mode_OUT);
#elif defined(STM32F1)
  gpioLed.init(GPIO_Mode_Out_PP);
#else
#error
#endif
}

int main() {
  configureClocks(1000);

  initGpioLed();

#if defined(USE_USART_1)
  initUsart1();
#endif

#if defined(USE_USART_2)
  initUsart2();
#endif

#if defined(USE_USART_3)
  initUsart3();
#endif
  printf("starting\r\n");
  Adc adc1{ADC1};
  AdcChannel *adcChan1;
  adcChan1 = adc1.addChannel(ADC_CHAN);
  adc1.init();
  adc1.enable();
  adc1.startConversion();

  while (1) {
    adc1.waitConversion();
    printf("adc1: %d\r\n", adcChan1->average);
    gpioLed.toggle();
  }

  return 0;
}
