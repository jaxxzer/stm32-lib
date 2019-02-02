#include "lookup.h"
#include "stm32lib-conf.h"
#include <math.h>

#define PI 3.141593f

#ifdef STM32F0
Timer &timer = timer3;
Gpio gpio_Led{GPIOB, 1};
TimerChannelOutput tco{TIM3, TIM_Channel_4};
#elif STM32F1
Timer &timer = timer3;
Gpio gpio_Led{GPIOB, 13};

Gpio gpio_m2{GPIOA, 7};
Gpio gpio_m3{GPIOB, 0};
Gpio gpio_m4{GPIOB, 1};

TimerChannelOutput tco1{TIM1, TIM_Channel_1};

TimerChannelOutput tco2{TIM3, TIM_Channel_2};
TimerChannelOutput tco3{TIM3, TIM_Channel_3};
TimerChannelOutput tco4{TIM3, TIM_Channel_4};
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
  tco1.setDuty(65535 - lookup[_field_angle]);
  tco2.setDuty(lookup[_field_angle]);
  tco3.setDuty(lookup[(_field_angle + 1366) % 4098]);
  tco4.setDuty(lookup[(_field_angle + 2372) % 4098]);
}

int main() {
  configureClocks(10);

#ifdef STM32F0
  gpio_Led.init(GPIO_Mode_AF);
  gpio_Led.configAF(1);
#elif STM32F1
  nvic_config(TIM1_UP_IRQn, 0, 0, ENABLE);

  gpio_Led.init(GPIO_Mode_AF_PP);
  gpio_m2.init(GPIO_Mode_AF_PP);
  gpio_m3.init(GPIO_Mode_AF_PP);
  gpio_m4.init(GPIO_Mode_AF_PP);
#endif

  timer1.initFreq(500); // 10kHz pwm frequency
  timer1.setEnabled(ENABLE);
  timer1.setMOE(ENABLE);
  timer1.setupUpCallback(&updateSPWM);
  timer1.interruptConfig(TIM_IT_Update, ENABLE);

  timer.initFreq(24e4); // 10kHz pwm frequency
  timer.setEnabled(ENABLE);
  timer.setMOE(ENABLE);

  tco1.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable, TIM_OutputNState_Enable);
  tco2.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable);
  tco3.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable);
  tco4.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable);

  while (1) {
  }

  return 0;
}
