#include "scheduling.h"
#include "stm32f0xx.h"

void configureClocks(uint32_t frequency)
{
	systick_frequency = frequency;
	SystemInit();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/systick_frequency); // Configure system interrupt
}

// Delay function for microsecond delay
void uDelay(uint32_t us) {
	volatile uint32_t tStart = MicroSeconds;
	while(MicroSeconds < tStart + us) asm volatile("nop");
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
		MicroSeconds += 1000000/systick_frequency; //Increment millisecond variable
	}
}
