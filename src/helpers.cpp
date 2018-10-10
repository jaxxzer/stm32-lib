#include "helpers.h"

#ifdef STM32F0
void nvic_config(const uint8_t irq, const uint8_t priority, const FunctionalState enabled)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = irq;
	NVIC_InitStructure.NVIC_IRQChannelPriority = priority;
	NVIC_InitStructure.NVIC_IRQChannelCmd = enabled;
	NVIC_Init(&NVIC_InitStructure);
}
#elif STM32F1
void nvic_config(const uint8_t irq, const uint8_t priority, const uint8_t subpriority, const FunctionalState enabled)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = priority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = subpriority;
	NVIC_InitStructure.NVIC_IRQChannelCmd = enabled;
	NVIC_Init(&NVIC_InitStructure);
}
#endif

// map float from input range to output range
float map(float in, float in_min, float in_max, float out_min, float out_max)
{
	if (in < in_min) {
		return out_min;
	}

	if (in > in_max) {
		return out_max;
	}

	return ((in - in_min) / (in_max - in_min)) * (out_max - out_min) + out_min;
}

// TODO Implement if necessary
#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

/*
 * Minimal __assert_func used by the assert() macro
 * */
extern "C" void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
//	while(1)
//	{ print("fuck");}
}

/*
 * Minimal __assert() uses __assert__func()
 * */
extern "C" void __assert(const char *file, int line, const char *failedexpr)
{
	__assert_func (file, line, nullptr, failedexpr);
}

#ifdef USE_SEE
#ifndef USE_DEFAULT_TIMEOUT_CALLBACK
/**
 * @brief  Basic management of the timeout situation.
 * @param  None.
 * @retval sEE_FAIL.
 */
uint32_t sEE_TIMEOUT_UserCallback(void)
{
	/* Return with error code */
	return sEE_FAIL;
}
#endif
#endif
