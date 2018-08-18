#pragma once

#include <inttypes.h>

extern volatile uint32_t MicroSeconds;
extern uint32_t systick_frequency;


void configureClocks(uint32_t frequency);
void uDelay(uint32_t us);
void mDelay(uint32_t ms);
void Delay(uint32_t s);
