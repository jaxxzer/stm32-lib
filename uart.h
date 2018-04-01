/*
 * uart.h
 *
 *  Created on: Feb 11, 2018
 *      Author: jack
 */

#pragma once

#include <stdio.h>
#include "stm32f0xx_conf.h"

class Uart
{
public:
	Uart(USART_TypeDef* usartx) : _peripheral(usartx) {
		setClockEnabled(ENABLE);
		USART_StructInit(&_config);
	}

	void write(char* p, uint16_t len);
	void write(char p);
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

//	typedef struct
//	{
//	  uint32_t USART_BaudRate;            /*!< This member configures the USART communication baud rate.
//	                                           The baud rate is computed using the following formula:
//	                                            - IntegerDivider = ((PCLKx) / (16 * (USART_InitStruct->USART_BaudRate)))
//	                                            - FractionalDivider = ((IntegerDivider - ((uint32_t) IntegerDivider)) * 16) + 0.5 */
//
//	  uint32_t USART_WordLength;          /*!< Specifies the number of data bits transmitted or received in a frame.
//	                                           This parameter can be a value of @ref USART_Word_Length */
//
//	  uint32_t USART_StopBits;            /*!< Specifies the number of stop bits transmitted.
//	                                           This parameter can be a value of @ref USART_Stop_Bits */
//
//	  uint32_t USART_Parity;              /*!< Specifies the parity mode.
//	                                           This parameter can be a value of @ref USART_Parity
//	                                           @note When parity is enabled, the computed parity is inserted
//	                                                 at the MSB position of the transmitted data (9th bit when
//	                                                 the word length is set to 9 data bits; 8th bit when the
//	                                                 word length is set to 8 data bits). */
//
//	  uint32_t USART_Mode;                /*!< Specifies wether the Receive or Transmit mode is enabled or disabled.
//	                                           This parameter can be a value of @ref USART_Mode */
//
//	  uint32_t USART_HardwareFlowControl; /*!< Specifies wether the hardware flow control mode is enabled
//	                                           or disabled.
//	                                           This parameter can be a value of @ref USART_Hardware_Flow_Control*/
//	}





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

void Uart::write(char ch) {
	// Code to write character 'ch' on the UART
	USART_SendData(USART1,ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

void Uart::write(char* ch, uint16_t len)
{
	for (uint16_t i = 0; i < len; i++) {
		write(*ch++);
	}
}

void Uart::cls(void) {
	write(27);       // ESC command
	printf("[2J");    // clear screen command
	write(27);
	printf("[H"); // goto home
}

void Uart::bkspc(void) {
	write(0x08);       // ESC command
}


