#include "exti.h"
#include "stm32lib-conf.h"

#define USART_BAUDRATE 115200

Gpio gpioLed{GPIOC, 13};

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

#if defined(STM32F0) || defined(STM32F3)
  gpioUsart1Rx.init(GPIO_Mode_AF, GPIO_PuPd_UP);
  gpioUsart1Tx.init(GPIO_Mode_AF, GPIO_PuPd_UP);
  gpioUsart1Rx.configAF(GPIO_USART1_RX_AF);
  gpioUsart1Tx.configAF(GPIO_USART1_TX_AF);
#elif defined(STM32F1)
  gpioUsart1Rx.init(GPIO_Mode_AF_PP);
  gpioUsart1Tx.init(GPIO_Mode_AF_PP);
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
GPIO_TypeDef *ports[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};

void extiCb(void) { gpioLed.toggle(); }

void setupGpio(uint8_t port, uint32_t pin) {
  if (((uint32_t)ports[port] == GPIOA_BASE && pin == 13) || ((uint32_t)ports[port] == GPIOA_BASE && pin == 14) ||
      ((uint32_t)ports[port] == GPIOA_BASE && pin == 9) || ((uint32_t)ports[port] == GPIOA_BASE && pin == 10)) {
    return;
  }
  Gpio gpioExti(ports[port], pin);
  gpioExti.init(GPIO_Mode_IN_FLOATING);

  gpioExti.extiConfig(ENABLE, EXTI_Trigger_Rising_Falling);
}

void disableGpio(uint8_t port, uint8_t pin) {
  if (((uint32_t)ports[port] == GPIOA_BASE && pin == 13) || ((uint32_t)ports[port] == GPIOA_BASE && pin == 14) ||
      ((uint32_t)ports[port] == GPIOA_BASE && pin == 9) || ((uint32_t)ports[port] == GPIOA_BASE && pin == 10)) {
    return;
  }
  Gpio gpioExti(ports[port], pin);
  gpioExti.extiConfig(DISABLE);
}

int main() {
  configureClocks(1000);

#if defined(STM32F0) || defined(STM32F3)
  gpioLed.init(GPIO_Mode_OUT);
#elif defined(STM32F1)
  gpioLed.init(GPIO_Mode_Out_PP);
#else
#error
#endif

#if defined(USE_USART_1)
  initUsart1();
#endif

#if defined(USE_USART_2)
  // uart2.dmaTxInit();
  // initUsart2();
#endif

#if defined(USE_USART_3)
  // initUsart3();
#endif

  exti.setupGlobalCallback(&extiCb);

  Gpio gp{GPIOA, 0};
  gp.init(GPIO_Mode_Out_PP);
  gp.reset();
  while (1) {
    for (uint8_t i = 0; i < 3; i++) {
      printf("testing %d\r\n", i);
      for (uint8_t j = 0; j < 16; j++) {
        setupGpio(i, j);
      }
      mDelay(3000);
      printf("disabling %d\r\n", i);
      for (uint8_t j = 0; j < 16; j++) {
        disableGpio(i, j);
      }
    }
  }

  // exti.setupCallback(8, &extiCb);
  // exti.initLine(8, EXTI_Trigger_Rising);

  while (1)
    ;
  while (1) {
#if defined(USE_USART_1)
    uart1.write("hello1 ", 7);
#endif

#if defined(USE_USART_2)
    uart2.write("hello2 ", 7);
#endif

#if defined(USE_USART_3)
    uart3.write("hello3 ", 7);
#endif
    mDelay(100);
    gpioLed.toggle();
  }

  return 0;
}
