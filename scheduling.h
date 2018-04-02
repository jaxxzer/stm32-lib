#pragma once

volatile uint32_t MicroSeconds = 0;
static uint32_t systick_prescaler; // millisecond interrupt

void configureClocks(uint32_t prescaler)
{
	systick_prescaler = prescaler;
	SystemInit();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/systick_prescaler); // 1ms system interrupt
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
		MicroSeconds += 1000000/systick_prescaler; //Increment millisecond variable
	}
}
