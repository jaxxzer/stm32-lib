#pragma once

#include <cstddef>
#include <stdio.h>
#include "stm32lib-conf.h"
#include "uart.h"

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
	int16_t __io_putchar(uint8_t* ch, uint32_t file);

	int __io_getchar(void);
int _write(int file, char *data, int len);
	// This is used by fprintf when only one character is printed, or no formating is used
	int fputc(int ch, FILE *f);
}

void print_clocks();

void printBits(size_t const size, volatile void * const ptr);


void print(const char* c);
void println(void);
void printHex(uint32_t i);
uint16_t my_atoi(char* c);

void print(uint16_t i);

void my_printInt(uint32_t i);
