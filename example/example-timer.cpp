#define USE_TIM1

#include "stm32lib.h"

// Timer update interrupt is used for 

Gpio gpio_Led { GPIOB, 1 };


void tim1UpdateCallback(void)
{
    gpio_Led.toggle();
}

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
    //Timer timer1  { TIM1 }; // 16 bit Advanced control

    gpio_Led.init(GPIO_Mode_OUT);

    nvic_config(TIM1_BRK_UP_TRG_COM_IRQn, 0, ENABLE);
    timer1.setClockEnabled(ENABLE);
    //*(uint32_t*)0x40021018 |= (1U<<14);
    timer1.initFreq(10); // 10Hz update
    timer1.setupUpCallback(&tim1UpdateCallback);
    timer1.setEnabled(ENABLE);
    timer1.interruptConfig(TIM_IT_Update, ENABLE);

    while (1) { 
        mDelay(1000);
     }

    return 0;
}