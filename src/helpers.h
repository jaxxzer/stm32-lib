#pragma once

#include "stm32lib-conf.h"
#include <inttypes.h>

#define UINT16MAX ( (1U << 16) - 1 )
// This file has some random helper and system functions
// printf goes to __io_putchar here
#if defined(STM32F0) 
void nvic_config(const uint8_t irq, const uint8_t priority, const FunctionalState enabled);
#elif defined(STM32F1) || defined(STM32F3)
void nvic_config(const uint8_t irq, const uint8_t priority, const uint8_t subpriority, const FunctionalState enabled);
#else
#error
#endif

// map float from input range to output range
float map(float in, float in_min, float in_max, float out_min, float out_max);

// TODO Implement if necessary
#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line);

#endif

/*
 * Minimal __assert_func used by the assert() macro
 * */
extern "C" void __assert_func(const char *file, int line, const char *func, const char *failedexpr);

/*
 * Minimal __assert() uses __assert__func()
 * */
extern "C" void __assert(const char *file, int line, const char *failedexpr);

#ifdef USE_SEE
#ifndef USE_DEFAULT_TIMEOUT_CALLBACK
/**
 * @brief  Basic management of the timeout situation.
 * @param  None.
 * @retval sEE_FAIL.
 */
uint32_t sEE_TIMEOUT_UserCallback(void);
#endif
#endif /* USE_SEE */
