#pragma once

#include "stm32f0xx_conf.h"
#include <inttypes.h>
#include "uart.h"

extern Uart uart;

void print(const char* c) {
	while (*c != 0) {
		uart.write(c++);
	}
}

void println(void)
{
	print("\n\r");
}
void printHex(uint32_t i)
{
	char buf[8];
	uint8_t p = 8;

	do {
		uint8_t digit = i%16;
		switch (digit) {
		case 0 ... 9:
			buf[--p] = '0' + digit;
			break;
		case 0xA ... 0xF:
			buf[--p] = 'A' + digit - 10;
			break;
		default:
			break;
		}
		i /= 16;
	} while (i);
	while (p--) {
		buf[p] = '0';
	}
	uart.write(buf, 8);
}

uint16_t my_atoi(char* c)
{
	uint16_t r = 0;
	while (*c) {
		uint8_t h = *(uint8_t*)c;
		uint8_t e = (uint8_t)'0';
		r = r * 10 + h - e;
		c++;
	}
	return r;
}

void print(uint16_t i) {
	char c[5];
	uint8_t len = 5;

	uint8_t p = i % 10;
	c[--len] = p + '0';
	i -= p; // not neccessary? taken care of with integer division

	while (i) {
		i /= 10;
		p = i % 10;
		c[--len] = p  + '0';
		i -= p; // not neccessary? taken care of with integer division
	}
	uart.write(&c[len], 5-len);
}
void my_printInt(uint32_t i) {
	char c[10];
	uint8_t len = 10;

	uint8_t p = i % 10;
	c[--len] = p + '0';
	i -= p; // not neccessary? taken care of with integer division

	while (i) {
		i /= 10;
		p = i % 10;
		c[--len] = p  + '0';
		i -= p; // not neccessary? taken care of with integer division
	}
	uart.write(&c[len], 10-len);
}

// This file has some random helper and system functions
// printf goes to __io_putchar here
void nvic_config(const uint8_t irq, const uint8_t priority, const FunctionalState enabled)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = irq;
	NVIC_InitStructure.NVIC_IRQChannelPriority = priority;
	NVIC_InitStructure.NVIC_IRQChannelCmd = enabled;
	NVIC_Init(&NVIC_InitStructure);
}

void print_clocks()
{

	RCC_ClocksTypeDef RCC_ClocksStruct;
	RCC_GetClocksFreq(&RCC_ClocksStruct);

	printf("\n\r\t***Current Clock Configuration***");

	printf("\n\r\t- SYSCLK: %u", RCC_ClocksStruct.SYSCLK_Frequency);
	printf("\n\r\t- HCLK: %u", RCC_ClocksStruct.HCLK_Frequency);
	printf("\n\r\t- PCLK: %u", RCC_ClocksStruct.PCLK_Frequency);
	printf("\n\r\t- ADCCLK: %u", RCC_ClocksStruct.ADCCLK_Frequency);
	printf("\n\r\t- CECCLK: %u", RCC_ClocksStruct.CECCLK_Frequency);
	printf("\n\r\t- I2C1CLK: %u", RCC_ClocksStruct.I2C1CLK_Frequency);
	printf("\n\r\t- USART1CLK: %u", RCC_ClocksStruct.USART1CLK_Frequency);
	printf("\n\r\t- USART2CLK: %u", RCC_ClocksStruct.USART2CLK_Frequency);
	printf("\n\r\t- USART3CLK: %u", RCC_ClocksStruct.USART3CLK_Frequency);
	printf("\n\r\t- USBCLK: %u", RCC_ClocksStruct.USBCLK_Frequency);
}

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

void printBits(size_t const size, volatile void * const ptr)
{
	unsigned char *b = (unsigned char*) ptr;
	unsigned char byte;
	int i, j;

	for (i=size-1;i>=0;i--)
	{
		for (j=7;j>=0;j--)
		{
			byte = (b[i] >> j) & 1;
			printf("%u", byte);
		}
		printf("__");
	}
	puts("");
}

enum {
	FD_NULL,
	FD_STDOUT,
	FD_STDERR,
	FD_USART1,
	FD_USART2,
	FD_USART3,
	FD_USART5,
	FD_USART6,
	FD_ITM_CH0,
	FD_ITM_CH1,
	FD_ITM_CH2,
	FD_ITM_CH3,
	FD_ITM_CH4,
	FD_ITM_CH5,
	FD_ITM_CH6,
	FD_ITM_CH7,
	FD_ITM_CH8,
	FD_ITM_CH9,
	FD_ITM_CH10,
	FD_ITM_CH11,
	FD_ITM_CH12,
	FD_ITM_CH13,
	FD_ITM_CH14,
	FD_ITM_CH15,
	FD_ITM_CH16,
	FD_ITM_CH17,
	FD_ITM_CH18,
	FD_ITM_CH19,
	FD_ITM_CH20,
	FD_ITM_CH21,
	FD_ITM_CH22,
	FD_ITM_CH23,
	FD_ITM_CH24,
	FD_ITM_CH25,
	FD_ITM_CH26,
	FD_ITM_CH27,
	FD_ITM_CH28,
	FD_ITM_CH29,
	FD_ITM_CH30,
	FD_ITM_CH31
};

extern "C" {

	// This is used by printf, which calls _write in syscalls.c
	int16_t __io_putchar(uint8_t* ch, uint32_t file) {
		switch(file) {
		case FD_STDOUT: // TODO find an easy way to map this (function pointer?) without modifying library
			uart.write((char*)ch);
			break;
		case FD_STDERR:
		case FD_USART1: // For example
			USART_SendData(USART1, *ch);
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); // blocking!! can prevent code to run
			break;
		case FD_ITM_CH0...FD_ITM_CH31: // For example
	#ifdef STM32F10X_MD
		ITM_SendCharPort(*ch, file - FD_ITM_CH0);
	#endif
		break;
		case FD_NULL:
		default:
			break;
		}

		{}
		return *ch;
	}

	int __io_getchar(void) {
		// Code to read a character from the UART
		return 0;
	}

	// This is used by fprintf when only one character is printed, or no formating is used
	int fputc(int ch, FILE *f)
	{
		return __io_putchar((uint8_t*)&ch, f->_file);
	}
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
	while(1)
	{}
}

/*
 * Minimal __assert() uses __assert__func()
 * */
extern "C" void __assert(const char *file, int line, const char *failedexpr)
{
	__assert_func (file, line, NULL, failedexpr);
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
#endif /* USE_SEE */
