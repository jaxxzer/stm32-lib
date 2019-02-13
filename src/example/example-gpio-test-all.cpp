#include "gpio.h"
#include "scheduling.h"
GPIO_TypeDef *ports[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};

void testPin(uint8_t port, uint8_t pin) {

  if (((uint32_t)ports[port] == GPIOA_BASE && pin == 13) || ((uint32_t)ports[port] == GPIOA_BASE && pin == 14)) {
    return;
  }

  Gpio gpio{ports[port], pin};

#if defined(STM32F0) || defined(STM32F3)
  gpio.init(GPIO_Mode_OUT);
#elif defined(STM32F1)
  gpio.init(GPIO_Mode_Out_PP);
#else
#error
#endif

  for (int i = 0; i < port + 1; i++) {
    gpio.reset();
    mDelay(1);
    gpio.set();
    mDelay(1);
  }
  gpio.reset();

  mDelay(5);

  for (int i = 0; i < pin + 1; i++) {
    gpio.reset();
    mDelay(1);
    gpio.set();
    mDelay(1);
  }
  gpio.reset();
}

int main(void) {
  configureClocks(1000);

  for (uint8_t i = 0; i < 5; i++) {
    for (uint8_t j = 0; j < 16; j++) {
      testPin(i, j);
    }
  }
  for (uint8_t i = 0; i < 5; i++) {
    for (uint8_t j = 0; j < 16; j++) {
      testPin(i, j);
    }
  }
  for (uint8_t i = 0; i < 5; i++) {
    for (uint8_t j = 0; j < 16; j++) {
      testPin(i, j);
    }
  }
  while (1) {
    // p2
    // pin 1
    for (uint8_t i = 0; i < 5; i++) {
      for (uint8_t j = 0; j < 16; j++) {
        testPin(i, j);
      }
    }
  }

  return 0;
}
