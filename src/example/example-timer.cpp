#define USE_TIM1

#include "stm32lib-conf.h"

// Timer update interrupt is used for 

    #ifdef STM32F0
    Gpio gpio_Led { GPIOB, 1 };
    #elif STM32F1
    Gpio gpio_Led { GPIOB, 13 };
    #endif

void tim1UpdateCallback(void)
{
    gpio_Led.toggle();
}

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

#ifdef STM32F0
    gpio_Led.init(GPIO_Mode_OUT);
#elif STM32F1
    gpio_Led.init(GPIO_Mode_Out_PP);
    //gpio_Led.configRemap();
#endif

#ifdef STM32F0
    nvic_config(TIM1_BRK_UP_TRG_COM_IRQn, 0, ENABLE);
#elif STM32F1
    nvic_config(TIM1_UP_IRQn, 0, 0, ENABLE);
#endif

    timer1.setClockEnabled(ENABLE);
    timer1.initFreq(10); // 10Hz update
    timer1.setupUpCallback(&tim1UpdateCallback);
    timer1.setEnabled(ENABLE);
    timer1.interruptConfig(TIM_IT_Update, ENABLE);

    while (1) {  }

    return 0;
}