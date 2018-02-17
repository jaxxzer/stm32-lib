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

//#include "stm32lib-conf.h"
#include <inttypes.h>

extern volatile uint32_t microseconds;
extern uint32_t systick_frequency;

void configureClocks(uint32_t frequency);

// microsecond delay
void uDelay(uint32_t us);

// millisecond delay
void mDelay(uint32_t ms);

// second delay
void Delay(uint32_t s);
