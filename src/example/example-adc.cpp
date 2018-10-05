#define USE_TIM_3

#include "stm32lib.h"

// Timer update interrupt is used for 

Gpio gpio_Led { GPIOB, 1 };
TimerChannelOutput tco_4 { TIM3, TIM_Channel_4 };

int main()
{
	SystemInit();

    // Set up 48 MHz Core Clock using HSI (4Mhz? - HSI_Div2) with PLL x 6
    RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_12);
    RCC_PLLCmd(ENABLE);

    // Wait for PLLRDY after enabling PLL.
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET) {  }

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  // Select the PLL as clock source.

	SystemCoreClockUpdate();

	// 1millisecond system interrupt
	SysTick_Config(SystemCoreClock/1000);
	systick_frequency = 1000; // todo fix this in hal somehow. this is needed when we are configured for internal clock?

    gpio_Led.init(GPIO_Mode_AF);
    gpio_Led.configAF(1);

    timer3.setClockEnabled(ENABLE);
    timer3.initFreq(1e4); // 10kHz pwm frequency
    timer3.setEnabled(ENABLE);

    tco_4.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable);
    uint16_t duty = 0;
    int8_t inc = 75;
    while (1) { 
        mDelay(1);
        tco_4.setDuty(duty);
        if ( (inc > 0 && inc > 65535 - duty) ||
             (inc < 0 && duty < -inc) )
        {
            inc = -inc;
        }       
        duty += inc;
     }

    return 0;
}