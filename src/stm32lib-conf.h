#pragma once
#include "target.h"

#if defined(STM32F0)
#include "stm32f0xx_conf.h"
#elif defined(STM32F1)
#include "stm32f10x_conf.h"
#elif defined(STM32F3)
#include "stm32f30x_conf.h"
#else
#error
#endif

typedef struct it_callback {
  void (*callback)(void);
  it_callback *next = nullptr;
} it_callback_t;


#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "exti.h"
#include "flash.h"
#include "gpio.h"
#include "helpers.h"
#include "printing.h"
#include "scheduling.h"
#include "timer.h"
#include "uart.h"
