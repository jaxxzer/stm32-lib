#pragma once


//#define USE_TIM_1
//#define USE_TIM_2
//#define USE_TIM_3
//#define USE_TIM_4
#ifdef STM32F0
#include "stm32f0xx_conf.h"
#elif STM32F1
#include "stm32f10x_conf.h"
#endif

typedef struct it_callback
{
	void (*callback)(void);
	it_callback* next = 0x0;
} it_callback_t;

#include "helpers.h"
#include "timer.h"
//#include "uart.h"
//#include "scheduling.h"
//#include "dma.h"
#include "gpio.h"
//#include "adc.h"
//#include "crc.h"
//#include "flash.h"