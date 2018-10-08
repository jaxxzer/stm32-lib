#include "stm32lib-conf.h"

// Timer update interrupt is used for 


    #ifdef STM32F0
    Timer& timer = timer3;
    Gpio gpio_Led { GPIOB, 1 };
    TimerChannelOutput tco { TIM3, TIM_Channel_4 };
    #elif STM32F1
    Timer& timer = timer1;
    Gpio gpio_Led { GPIOB, 13 };
    TimerChannelOutput tco { TIM1, TIM_Channel_1 };
    #endif

int main()
{


	SystemInit();

    // Set up 48 MHz Core Clock using HSI (4Mhz? - HSI_Div2) with PLL x 6
    #ifdef STM32F0
    RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_12);
    #elif STM32F1
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
    #endif
        RCC_PLLCmd(ENABLE);

    // Wait for PLLRDY after enabling PLL.
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET) {  }

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  // Select the PLL as clock source.

	SystemCoreClockUpdate();

	// 1millisecond system interrupt
	SysTick_Config(SystemCoreClock/1000);
	systick_frequency = 1000; // todo fix this in hal somehow. this is needed when we are configured for internal clock?

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
#ifdef STM32F0
    gpio_Led.init(GPIO_Mode_AF);
    gpio_Led.configAF(1);
#elif STM32F1
    gpio_Led.init(GPIO_Mode_AF_PP);
    //gpio_Led.configRemap();
#endif

    timer.setClockEnabled(ENABLE);
    timer.initFreq(1e4); // 10kHz pwm frequency
    timer.setEnabled(ENABLE);


#ifdef STM32F0
    tco.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable);
#elif STM32F1
    tco.init(TIM_OCMode_PWM1, 0, TIM_OutputState_Enable, TIM_OutputNState_Enable);
#endif
    uint16_t duty = 0;
    int8_t inc = 75;
    while (1) { 
        mDelay(1);
        tco.setDuty(duty);
        if ( (inc > 0 && inc > 65535 - duty) ||
             (inc < 0 && duty < -inc) )
        {
            inc = -inc;
        }       
        duty += inc;
     }

    return 0;
}