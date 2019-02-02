#include "printing.h"

extern "C" {
	// This is used by printf, which calls _write in syscalls.c
	int16_t __io_putchar(uint8_t* ch, uint32_t file) {
		switch(file) {
		case FD_STDOUT: // TODO find an easy way to map this (function pointer?) without modifying library
			#if defined(STDOUT_USART)
				STDOUT_USART.write((char*)ch);
			#endif
			break;
		case FD_STDERR:
		case FD_USART1: // For example
#ifdef USE_USART1
			uart1.write((char*)ch);
#endif
			break;
		case FD_USART2: // For example
#ifdef USE_USART2
			uart2.write((char*)ch);
#endif
			break;
		case FD_USART3: // For example
#ifdef USE_USART3
			uart3.write((char*)ch);
#endif
			break;
		case FD_ITM_CH0...FD_ITM_CH31: // For example
		//ITM_SendCharPort(*ch, file - FD_ITM_CH0);
		break;
		case FD_NULL:
		default:
			break;
		}

		{}
		return *ch;
	}

	int _write(int file, char* data, int len)	{
		while (len--) {
			__io_putchar((uint8_t*)data++, file);
		}
		return 0;
	}

	int __io_getchar(void) {
		// Code to read a character from the UART
		return 0;
	}

	//This is used by fprintf when only one character is printed, or no formating is used
	int fputc(int ch, FILE *f)
	{
		return __io_putchar((uint8_t*)&ch, f->_file);
		return 0;
	}

	int fputs(const char* str, FILE* stream) {
		while (str) {
			fputc(*(str++), stream);
		}
		return 0;
	}
}

void print_clocks()
{
	RCC_ClocksTypeDef RCC_ClocksStruct;
	RCC_GetClocksFreq(&RCC_ClocksStruct);

	printf("\t***Current Clock Configuration***\r\n");
#if defined(STM32F0)
	printf("\t- SYSCLK: %lu\r\n", RCC_ClocksStruct.SYSCLK_Frequency);
	printf("\t- HCLK: %lu\r\n", RCC_ClocksStruct.HCLK_Frequency);
	printf("\t- PCLK: %lu\r\n", RCC_ClocksStruct.PCLK_Frequency);
	printf("\t- ADCCLK: %lu\r\n", RCC_ClocksStruct.ADCCLK_Frequency);
	printf("\t- CECCLK: %lu\r\n", RCC_ClocksStruct.CECCLK_Frequency);
	printf("\t- I2C1CLK: %lu\r\n", RCC_ClocksStruct.I2C1CLK_Frequency);
	printf("\t- USART1CLK: %lu\r\n", RCC_ClocksStruct.USART1CLK_Frequency);
	printf("\t- USART2CLK: %lu\r\n", RCC_ClocksStruct.USART2CLK_Frequency);
	printf("\t- USART3CLK: %lu\r\n", RCC_ClocksStruct.USART3CLK_Frequency);
	printf("\t- USBCLK: %lu\r\n", RCC_ClocksStruct.USBCLK_Frequency);
#elif defined(STM32F3)
	printf("\t- SYSCLK: %lu\r\n", RCC_ClocksStruct.SYSCLK_Frequency);
	printf("\t- HCLK: %lu\r\n", RCC_ClocksStruct.HCLK_Frequency);
	printf("\t- PCLK1: %lu\r\n", RCC_ClocksStruct.PCLK1_Frequency);
	printf("\t- PCLK2: %lu\r\n", RCC_ClocksStruct.PCLK2_Frequency);
	printf("\t- ADC12CLK: %lu\r\n", RCC_ClocksStruct.ADC12CLK_Frequency);
	printf("\t- ADC34: %lu\r\n", RCC_ClocksStruct.ADC34CLK_Frequency);
	printf("\t- I2C1: %lu\r\n", RCC_ClocksStruct.I2C1CLK_Frequency);
	printf("\t- I2C2: %lu\r\n", RCC_ClocksStruct.I2C2CLK_Frequency);
	printf("\t- I2C3: %lu\r\n", RCC_ClocksStruct.I2C3CLK_Frequency);
	printf("\t- TIM1: %lu\r\n", RCC_ClocksStruct.TIM1CLK_Frequency);
	printf("\t- HRTIM1: %lu\r\n", RCC_ClocksStruct.HRTIM1CLK_Frequency);
	printf("\t- TIM8: %lu\r\n", RCC_ClocksStruct.TIM8CLK_Frequency);
	printf("\t- TIM2: %lu\r\n", RCC_ClocksStruct.TIM2CLK_Frequency);
	printf("\t- TIM3: %lu\r\n", RCC_ClocksStruct.TIM3CLK_Frequency);
	printf("\t- USART1CLK: %lu\r\n", RCC_ClocksStruct.USART1CLK_Frequency);
	printf("\t- USART2CLK: %lu\r\n", RCC_ClocksStruct.USART2CLK_Frequency);
	printf("\t- USART3CLK: %lu\r\n", RCC_ClocksStruct.USART3CLK_Frequency);
	printf("\t- UART4: %lu\r\n", RCC_ClocksStruct.UART4CLK_Frequency);
	printf("\t- UART5: %lu\r\n", RCC_ClocksStruct.UART5CLK_Frequency);
	printf("\t- TIM15: %lu\r\n", RCC_ClocksStruct.TIM15CLK_Frequency);
	printf("\t- TIM16: %lu\r\n", RCC_ClocksStruct.TIM16CLK_Frequency);
	printf("\t- TIM17: %lu\r\n", RCC_ClocksStruct.TIM17CLK_Frequency);
	printf("\t- TIM20: %lu\r\n", RCC_ClocksStruct.TIM20CLK_Frequency);
#else
	printf("\t- SYSCLK: %lu\r\n", RCC_ClocksStruct.SYSCLK_Frequency);
	printf("\t- HCLK: %lu\r\n", RCC_ClocksStruct.HCLK_Frequency);
	printf("\t- PCLK1: %lu\r\n", RCC_ClocksStruct.PCLK1_Frequency);
	printf("\t- PCLK2: %lu\r\n", RCC_ClocksStruct.PCLK2_Frequency);
	printf("\t- ADC12CLK: %lu\r\n", RCC_ClocksStruct.ADCCLK_Frequency);
#endif
}


// void printBits(size_t const size, volatile void * const ptr)
// {
// 	unsigned char *b = (unsigned char*) ptr;
// 	unsigned char byte;
// 	int i, j;

// 	for (i=size-1;i>=0;i--)
// 	{
// 		for (j=7;j>=0;j--)
// 		{
// 			byte = (b[i] >> j) & 1;
// 			printf("%u", byte);
// 		}
// 		printf("__");
// 	}
// 	puts("");
// }

void print(const char* c) {
	while (*c != 0) {
		_write(FD_STDOUT, (char*)c++, 1);
	}
}

void println(void)
{
	print("\r\n");
}

void printHex(uint32_t i)
{
	uint8_t size = 1;
	if (i > 0xF) {
		size = 2;
	}
	if (i > 0xFF) {
		size = 4;
	}
	if (i > 0xFFFF) {
		size = 8;
	}

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
	while (p > 8 - size) {
		buf[--p] = '0';
	}
	_write(FD_STDOUT, &buf[p], size);
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

// void print(uint16_t i) {
// 	char c[5];
// 	uint8_t len = 5;

// 	uint8_t p = i % 10;
// 	c[--len] = p + '0';
// 	i -= p; // not neccessary? taken care of with integer division

// 	while (i) {
// 		i /= 10;
// 		p = i % 10;
// 		c[--len] = p  + '0';
// 		i -= p; // not neccessary? taken care of with integer division
// 	}
// 	uart1.write(&c[len], 5-len);
// }

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
	_write(FD_STDOUT, &c[len], 10-len);
}
