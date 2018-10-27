#pragma once

#if defined(STM32F0)
#include "stm32f0xx_conf.h"
#elif defined(STM32F1)
#include "stm32f10x_conf.h"
#elif defined(STM32F3)
#include "stm32f30x_conf.h"
#else
#error
#endif

typedef struct it_callback
{
	void (*callback)(void);
	it_callback* next = nullptr;
} it_callback_t;
//#define USE_USART_1
//#define USE_USART_3
//#define USE_TIM_1
//#define USE_TIM_3
#include "helpers.h"
#include "timer.h"
#include "uart.h"
//#include "printing.h"
#include "scheduling.h"
#include "dma.h"
#include "gpio.h"
#include "adc.h"
//#include "crc.h"
//#include "flash.h"

