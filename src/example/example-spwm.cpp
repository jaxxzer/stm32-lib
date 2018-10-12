#include "stm32lib-conf.h"
#include <math.h>

#define PI 3.141593f

#ifdef STM32F0
    Timer& timer = timer3;
    Gpio gpio_Led { GPIOB, 1 };
    TimerChannelOutput tco { TIM3, TIM_Channel_4 };
#elif STM32F1
    Timer& timer = timer3;
    Gpio gpio_Led { GPIOB, 13 };

    Gpio gpio_m2 { GPIOA, 7 };
    Gpio gpio_m3 { GPIOB, 0 };
    Gpio gpio_m4 { GPIOB, 1 };

    TimerChannelOutput tco1 { TIM1, TIM_Channel_1 };

    TimerChannelOutput tco2 { TIM3, TIM_Channel_2 };
    TimerChannelOutput tco3 { TIM3, TIM_Channel_3 };
    TimerChannelOutput tco4 { TIM3, TIM_Channel_4 };
#endif

float _field_angle = 0;
uint16_t _duty = 8000;

bool setFieldAngle(float angle)
{
   	while (angle > 2 * PI) {
   		angle -= 2 * PI;
   	}

   	_field_angle = angle;

   	return true;
}

void updateSPWM(void)
{
	float pwmA = (sinf(_field_angle) + 1) / 2;
	float pwmB = (sinf(_field_angle + 2.094394667f) + 1) / 2;
	float pwmC = (sinf(_field_angle + 4.188789333f) + 1) / 2;

	tco1.setDuty(65535 - pwmA * _duty);
	tco2.setDuty(pwmA * _duty);
	tco3.setDuty(pwmB * _duty);
	tco4.setDuty(pwmC * _duty);
}

int main()
{
    configureClocks(1000);

#ifdef STM32F0
    gpio_Led.init(GPIO_Mode_AF);
    gpio_Led.configAF(1);
#elif STM32F1
    gpio_Led.init(GPIO_Mode_AF_PP);
    gpio_m2.init(GPIO_Mode_AF_PP);
    gpio_m3.init(GPIO_Mode_AF_PP);
    gpio_m4.init(GPIO_Mode_AF_PP);
#endif

    timer1.initFreq(1e4); // 10kHz pwm frequency
    timer1.setEnabled(ENABLE);
    timer1.setMOE(ENABLE);


    timer.initFreq(1e4); // 10kHz pwm frequency
    timer.setEnabled(ENABLE);
    timer.setMOE(ENABLE);

    tco1.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable, TIM_OutputNState_Enable);
    tco2.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable);
    tco3.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable);
    tco4.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable);

    // breath
    uint16_t duty = 0;
    int8_t inc = 75;
    while (1) { 
        mDelay(10);
        setFieldAngle(_field_angle + 0.1f);
        updateSPWM();
    }

    return 0;
}
