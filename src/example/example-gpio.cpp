#include "stm32lib-conf.h"

Gpio gpio_Led { GPIOB, 1 };

static const int delay_ms = 100;

int main(void)
{
	SystemInit();

    // Set up 48 MHz Core Clock using HSI (4Mhz? - HSI_Div2) with PLL x 6
    RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_12);
    RCC_PLLCmd(ENABLE);

    // Wait for PLLRDY after enabling PLL.
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET)
    { }

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  // Select the PLL as clock source.

	SystemCoreClockUpdate();

	// 1millisecond system interrupt
	SysTick_Config(SystemCoreClock/1000);
	systick_frequency = 1000; // todo fix this in hal somehow. this is needed when we are configured for internal clock?

#ifdef STM32F0
    gpio_Led.init(GPIO_Mode_OUT);
#elif STM32F1
    gpio_Led.init(GPIO_Mode_Out_PP);
#endif
    while (1) {
        gpio_Led.toggle();
        mDelay(delay_ms);
    }

    return 0;
}
