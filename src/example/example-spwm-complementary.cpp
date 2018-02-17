#include "lookup.h"
#include "stm32lib-conf.h"
#include <math.h>

#define PI 3.141593f

#if defined(STM32F0)
#error
#elif defined(STM32F1)
#error
#elif defined(STM32F3)
Timer &timer = timer1;
Gpio gpio_m1n{GPIOA, 7};

Gpio gpio_m2{GPIOA, 9};
Gpio gpio_m2n{GPIOB, 0};
Gpio gpio_m3{GPIOA, 10};
Gpio gpio_m3n{GPIOB, 1};
Gpio gpio_m4{GPIOA, 11};

TimerChannelOutput tco1{&timer, TIM_Channel_1};
TimerChannelOutput tco2{&timer, TIM_Channel_2};
TimerChannelOutput tco3{&timer, TIM_Channel_3};
TimerChannelOutput tco4{&timer, TIM_Channel_4};
#else
#error
#endif

uint16_t _field_angle = 0;
uint16_t _duty = 12000;

bool setFieldAngle(float angle) {
  while (angle > 2 * M_PI) {
    angle -= 2 * M_PI;
    if (angle < 0)
      angle = 0;
  }

  _field_angle = angle;

  return true;
}

void updateSPWM(void) {
  _field_angle += 1;
  _field_angle = _field_angle % 4098;
  tco1.setDuty(lookup[(_field_angle + 2372) % 4098]);
  tco2.setDuty(lookup[_field_angle]);
  tco3.setDuty(lookup[(_field_angle + 1366) % 4098]);
  tco4.setDuty(lookup[(_field_angle + 2372) % 4098]);
}

int main() {
  configureClocks(10);

#if defined(STM32F0)
#error
#elif defined(STM32F3)
  nvic_config(TIM1_BRK_UP_TRG_COM_IRQn, 0, 0, ENABLE);

  gpio_m1n.init(GPIO_Mode_AF);
  gpio_m2.init(GPIO_Mode_AF);
  gpio_m2n.init(GPIO_Mode_AF);
  gpio_m3.init(GPIO_Mode_AF);
  gpio_m3n.init(GPIO_Mode_AF);
  gpio_m4.init(GPIO_Mode_AF);

  gpio_m1n.configAF(2);
  gpio_m2.configAF(2);
  gpio_m2n.configAF(2);
  gpio_m3.configAF(2);
  gpio_m3n.configAF(2);
  gpio_m4.configAF(2);
#else
#error
#endif

  timer.initFreq(24e4); // 10kHz pwm frequency
                        // timer.setDTG(0x3);
  TIM1->BDTR |= 0xe;
  TIM1->CR1 |= TIM_CR1_CMS_0;
  timer.setEnabled(ENABLE);

  timer.setMOE(ENABLE);

  tco1.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable, TIM_OutputNState_Enable);
  tco2.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable, TIM_OutputNState_Enable);
  tco3.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable, TIM_OutputNState_Enable);
  tco4.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable);

  tco1.setDuty(34000);
  tco2.setDuty(32000);
  tco3.setDuty(32000);
  tco4.setDuty(32000);
  while (1) {
    // updateSPWM();
    // mDelay(10);
  }

  return 0;
}
