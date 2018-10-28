#include "stm32lib-conf.h"

// Blue Pill
//Gpio gpio_Led { GPIOC, 13 }; // Blue Pill
Gpio gpio_Led { GPIOC, 9 }; // Ping

static const int delay_ms = 50;

int main(void)
{
	SystemInit();

    // Set up 48 MHz Core Clock using HSI (4Mhz? - HSI_Div2) with PLL x 6
#ifdef defined(STM32F0) || defined(STM32F3)
    RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_12);
#elif defined(STM32F1) 
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_12);
#endif

    RCC_PLLCmd(ENABLE);

    // Wait for PLLRDY after enabling PLL.
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET)
    { }

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  // Select the PLL as clock source.

	SystemCoreClockUpdate();

	// 1millisecond system interrupt
	SysTick_Config(SystemCoreClock/1000);
	systick_frequency = 1000; // todo fix this in hal somehow. this is needed when we are configured for internal clock?

#if defined(STM32F0) || defined(STM32F3)
    gpio_Led.init(GPIO_Mode_OUT);
#elif defined(STM32F1)
    gpio_Led.init(GPIO_Mode_Out_PP);
#else
#error
#endif

    while (1) {
        gpio_Led.toggle();
        mDelay(delay_ms);
    }

    return 0;
}
