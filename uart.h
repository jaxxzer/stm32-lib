#pragma once

#include <stdio.h>
#include "stm32f0xx_conf.h"
#include "scheduling.h"

class Uart
{
public:
	Uart(USART_TypeDef* usartx) : _peripheral(usartx) {
		setClockEnabled(ENABLE);
		USART_StructInit(&_config);
	}

	void write(char* p, uint16_t len);
	void write(char* p);
	void cls(void);
	void bkspc(void);
	char read();
	void ITConfig(uint32_t it, FunctionalState enabled);

	void init(uint32_t baudrate,
				uint32_t stopbits = USART_StopBits_1,
				uint32_t wordlength = USART_WordLength_8b,
				uint32_t parity = USART_Parity_No,
				uint32_t mode = USART_Mode_Rx | USART_Mode_Tx,
				uint32_t flowcontrol = USART_HardwareFlowControl_None)
	{
		_config.USART_BaudRate = baudrate;
		_config.USART_StopBits = stopbits;
		_config.USART_WordLength = wordlength;
		_config.USART_Parity;
		_config.USART_Mode;
		_config.USART_HardwareFlowControl = flowcontrol;
		init();
	}

	void set_baudrate(uint32_t baudrate)
	{
		_config.USART_BaudRate = baudrate;
	}

	// Start auto baud rate detection
	// Requires the next byte received to start with a 1
	void startAutoBaud() {
		USART_AutoBaudRateConfig(_peripheral, USART_AutoBaudRate_StartBit);
		USART_AutoBaudRateCmd(_peripheral, ENABLE);
	}

	void init() {
		USART_Init(_peripheral, &_config);
	}

	void setEnabled(FunctionalState enabled) {
		USART_Cmd(_peripheral, enabled);
	}

	void setClockEnabled(FunctionalState enabled);

private:
	USART_TypeDef* _peripheral;

	USART_InitTypeDef _config;

	static const uint8_t buf_size = 128;

	 uint16_t rxbuf[buf_size];
	 uint16_t txbuf[buf_size];

	// circular buffer pointers
	uint16_t* txStart;
	uint16_t* txEnd;
	uint16_t* rxStart;
	uint16_t* rxEnd;
};

void Uart::setClockEnabled(FunctionalState enabled)
{
	switch((uint32_t)_peripheral) {
	case USART1_BASE:
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, enabled);
		break;
	default:
		break;

	}
}

void Uart::ITConfig(uint32_t it, FunctionalState enabled)
{
	USART_ITConfig(_peripheral, it, enabled);
}

void Uart::write(char* ch) {
	// Code to write character 'ch' on the UART
	USART_SendData(USART1, *ch);
	uint32_t tstart = MicroSeconds;
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET && (MicroSeconds < tstart + 50));
}

void Uart::write(char* ch, uint16_t len)
{
	for (uint16_t i = 0; i < len; i++) {
		write(ch++);
	}
}

void Uart::cls(void) {
	static const uint8_t esc = 27;
	write((char*)(&esc));       // ESC command
	printf("[2J");    // clear screen command
	write((char*)(&esc));
	printf("[H"); // goto home
}

void Uart::bkspc(void) {
	static const uint8_t bkspc = 0x08;
	write((char*)(&bkspc));       // ESC command
}
