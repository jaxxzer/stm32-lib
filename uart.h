#pragma once

#include <stdio.h>
#include "stm32f0xx_conf.h"
#include "scheduling.h"

class Uart
{
public:
	Uart(USART_TypeDef* usartx) : _peripheral(usartx) {
		setClockEnabled(ENABLE);
		//USART_StructInit(&_config);
	}

	void write(const char* p, uint16_t len);
	void write(const char* p);

	int read(void);
	uint8_t rxWaiting(void);

	void cls(void);
	void bkspc(void);
	void ITConfig(uint32_t it, FunctionalState enabled);

	uint8_t txSpaceUsed(void);
	uint8_t txSpaceAvailable(void);
	void init(uint32_t baudrate,
				uint32_t stopbits = USART_StopBits_1,
				uint32_t wordlength = USART_WordLength_8b,
				uint32_t parity = USART_Parity_No,
				uint32_t mode = USART_Mode_Rx | USART_Mode_Tx,
				uint32_t flowcontrol = USART_HardwareFlowControl_None)
	{
		USART_InitTypeDef _config;

		_config.USART_BaudRate = baudrate;
		_config.USART_StopBits = stopbits;
		_config.USART_WordLength = wordlength;
		_config.USART_Parity = parity;
		_config.USART_Mode = mode;
		_config.USART_HardwareFlowControl = flowcontrol;

		USART_Init(_peripheral, &_config);
	}


	// Start auto baud rate detection
	// Requires the next byte received to start with a 1
	void startAutoBaud() {
		USART_AutoBaudRateConfig(_peripheral, USART_AutoBaudRate_StartBit);
		USART_AutoBaudRateCmd(_peripheral, ENABLE);
	}


	void setEnabled(FunctionalState enabled) {
		USART_Cmd(_peripheral, enabled);
	}

	void setClockEnabled(FunctionalState enabled);

	static const uint8_t bufSize = 64;
	char rxBuf[bufSize];
	char txBuf[bufSize];

	uint8_t rxHead = 1;
	uint8_t rxTail;
	uint8_t rxOverruns;

	uint8_t txHead;
	uint8_t txTail;
	uint8_t txOverruns;

	USART_TypeDef* _peripheral;
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

uint8_t Uart::txSpaceUsed(void)
{
	return ((uint8_t)(txTail - txHead) % bufSize);
}

uint8_t Uart::txSpaceAvailable(void)
{
	return bufSize - txSpaceUsed() - 1;
}

void Uart::write(const char* ch) {
	while (!txSpaceAvailable()) {
		txOverruns++; // block when buffer is full
	}

	txBuf[txTail++] = *ch;
	txTail = txTail % bufSize;

	_peripheral->CR1 |= USART_CR1_TXEIE;
}

void Uart::write(const char* ch, uint16_t len)
{
	for (uint16_t i = 0; i < len; i++) {
		write(ch++);
	}
}

uint8_t Uart::rxWaiting(void)
{
	return ((uint8_t)(rxTail - rxHead) % bufSize);
}

int Uart::read(void)
{
	if (!rxWaiting()) {
		return -1;
	}
	uint8_t bufIndex = rxHead++;
	rxHead = rxHead % bufSize;
	return rxBuf[bufIndex];
}

void Uart::cls(void)
{
	static const uint8_t esc = 27;
	write((char*)(&esc));       // ESC command
	printf("[2J");    // clear screen command
	write((char*)(&esc));
	printf("[H"); // goto home
}

void Uart::bkspc(void)
{
	static const uint8_t bkspc = 0x08;
	write((char*)(&bkspc));       // ESC command
}
