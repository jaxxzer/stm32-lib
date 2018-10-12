#pragma once

#ifdef STM32F0
#include "stm32f0xx_conf.h"
#elif STM32F1
#include "stm32f10x_conf.h"
#endif

typedef struct it_callback
{
	void (*callback)(void);
	it_callback* next = nullptr;
} it_callback_t;

#include "helpers.h"
#include "timer.h"
//#include "uart.h"
#include "scheduling.h"
//#nclude "dma.h"
#include "gpio.h"
//#include "adc.h"
//#include "crc.h"
//#include "flash.h"