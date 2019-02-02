#include "stm32lib-conf.h"

#define GPIO_USART1_TX GPIOA
#define PIN_USART1_TX 9

#define GPIO_USART1_RX GPIOA
#define PIN_USART1_RX 10

Gpio gpioUsart1Tx{GPIO_USART1_TX, PIN_USART1_TX};
Gpio gpioUsart1Rx{GPIO_USART1_RX, PIN_USART1_RX};

static uint16_t transferbyte = 0b1111100001100000;
static volatile uint16_t bitpos = 1;

#if defined(STM32F0)
#define GPIO_PING_AF 1
#define GPIO_LED_AF 1

Timer &timerPingDrive = timer3;
Gpio gpioLed{GPIOB, 1};
Gpio gpioPingDrive{GPIOA, 7};

TimerChannelOutput tcoLed{TIM3, TIM_Channel_4};
TimerChannelOutput tcoPingDrive{TIM3, TIM_Channel_2};

Timer &timerPingInterval = timer1;
TimerChannelOutput tcoPingDuration{TIM1, TIM_Channel_1};
#elif defined(STM32F1)

#elif defined(STM32F3)
#define GPIO_PING_AF 2
#define GPIO_LED_AF 2
Timer &timerPingDrive = timer3;
Gpio gpioLed{GPIOC, 9};
Gpio gpioPingDrive{GPIOC, 6};

TimerChannelOutput tcoLed{TIM3, TIM_Channel_4};
TimerChannelOutput tcoPingDrive{TIM3, TIM_Channel_1};

Timer &timerPingInterval = timer1;
TimerChannelOutput tcoPingDuration{TIM1, TIM_Channel_1};
#else
#error
#endif

void startPing() {
  if (transferbyte & (1 << bitpos)) {
    tcoPingDuration.setDuty(40000);
  } else {
    tcoPingDuration.setDuty(10000);
  }
  bitpos++;
  bitpos = bitpos % 16;
  printf("%d\n", bitpos);
  tcoPingDrive.setEnabled(ENABLE);
  tcoLed.setEnabled(ENABLE);
}

void endPing() {
  tcoPingDrive.setEnabled(DISABLE);
  tcoLed.setEnabled(DISABLE);
}

void initGpio() {
  gpioLed.init(GPIO_Mode_AF);
  gpioLed.configAF(GPIO_LED_AF);

  gpioPingDrive.init(GPIO_Mode_AF);
  gpioPingDrive.configAF(GPIO_PING_AF);
}

void initTimers() {
  timerPingDrive.initFreq(115e3); // 115kHz pwm frequency
  timerPingDrive.setEnabled(ENABLE);
  timerPingDrive.setMOE(ENABLE);

  tcoPingDrive.init(TIM_OCMode_PWM1);
  tcoLed.init(TIM_OCMode_PWM1);

  tcoPingDrive.setDuty(UINT16_MAX / 2);
  tcoLed.setDuty(UINT16_MAX / 2);

#if defined(STM32F1) || defined(STM32F3)
  nvic_config(TIM1_UP_TIM16_IRQn, 0, 0, ENABLE);
  nvic_config(TIM1_CC_IRQn, 0, 0, ENABLE);
#elif defined(STM32F0)
  nvic_config(TIM1_BRK_UP_TRG_COM_IRQn, 0, ENABLE);
  nvic_config(TIM1_CC_IRQn, 0, ENABLE);
#else
#error
#endif

  timerPingInterval.interruptConfig(TIM_IT_Update, ENABLE);
  timerPingInterval.interruptConfig(TIM_IT_CC1, ENABLE);

  timerPingInterval.setupUpCallback(&startPing);
  timerPingInterval.setupCc1Callback(&endPing);

  timerPingInterval.initFreq(1000); // 115kHz pwm frequency
  timerPingInterval.setEnabled(ENABLE);
  timerPingInterval.setMOE(ENABLE);

  tcoPingDuration.init(TIM_OCMode_PWM1);
  tcoPingDuration.setDuty(35000);
}

void initUsart1(void) {
#if defined(STM32F0)
  gpioUsart1Rx.init(GPIO_Mode_AF, GPIO_PuPd_UP);
  gpioUsart1Tx.init(GPIO_Mode_AF, GPIO_PuPd_UP);
  gpioUsart1Rx.configAF(1);
  gpioUsart1Tx.configAF(1);
  nvic_config(USART1_IRQn, 0, ENABLE);
#elif defined(STM32F1)
  gpioUsart1Rx.init(GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
  gpioUsart1Tx.init(GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  nvic_config(USART1_IRQn, 0, 0, ENABLE);
#elif defined(STM32F3)
  gpioUsart1Rx.init(GPIO_Mode_AF, GPIO_PuPd_UP);
  gpioUsart1Tx.init(GPIO_Mode_AF, GPIO_PuPd_UP);
  gpioUsart1Rx.configAF(7);
  gpioUsart1Tx.configAF(7);
  nvic_config(USART1_IRQn, 0, 0, ENABLE);
#else
#error
#endif
  uart1.init(3000000);
  uart1.ITConfig(USART_IT_RXNE, ENABLE);
  uart1.setEnabled(ENABLE);
  uart1.cls();
}

int main() {
  configureClocks(1000);

  initGpio();
  initUsart1();
  mDelay(10);
  uart1.write("hello", 5);
  initTimers();

  while (1) {
  };

  return 0;
}
