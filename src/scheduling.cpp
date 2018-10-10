#include "scheduling.h"

volatile uint32_t microseconds = 0;
uint32_t systick_frequency;
void configureClocks(uint32_t frequency)
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
	SysTick_Config(SystemCoreClock/frequency);
	systick_frequency = frequency;
}

// Delay function for microsecond delay
void uDelay(uint32_t us) {
	volatile uint32_t tStart = microseconds;
	while(microseconds < tStart + us) asm volatile("nop");
}

// Delay function for millisecond delay
void mDelay(uint32_t ms){
	uDelay(ms * 1000);
}

// Delay function for second delay
void Delay(uint32_t s){
	uDelay(s * 1000000);
}

extern "C" {
	void SysTick_Handler(void){
		microseconds += 1000000/systick_frequency; //Increment millisecond variable
	}
}
