#pragma once
#include <inttypes.h>
volatile uint32_t MicroSeconds = 0;
static uint32_t systick_frequency;


void configureClocks(uint32_t frequency);
void uDelay(uint32_t us);
void mDelay(uint32_t ms);
void Delay(uint32_t s);
