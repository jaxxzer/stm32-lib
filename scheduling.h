#pragma once

volatile uint32_t MicroSeconds = 0;
static uint32_t systick_prescaler = 1000; // millisecond interrupt

void configureClocks(uint32_t prescaler)
{
	systick_prescaler = prescaler;
	SystemInit();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/systick_prescaler); // 1ms system interrupt
}

void DelayMicros(uint32_t us) {
	volatile uint32_t tStart = MicroSeconds;
	while(MicroSeconds < tStart + us) asm volatile("nop");
}

// Thanks https://hsel.co.uk/2014/06/20/stm32f0-tutorial-2-systick/
//Delay function for millisecond delay
void DelayMil(uint32_t MilS){
	DelayMicros(MilS*1000);
}

//Delay function for second delay
void DelaySec(uint32_t S){
	DelayMicros(S*1000000);
}

#ifdef __cplusplus
 extern "C" {
#endif
 void SysTick_Handler(void){
	 	MicroSeconds += 1000000/systick_prescaler; //Increment millisecond variable
 }
#ifdef __cplusplus
 }
#endif
