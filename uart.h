/*
 * uart.h
 *
 *  Created on: Feb 11, 2018
 *      Author: jack
 */

#pragma once
  #include "stm32f0xx_conf.h"



class Uart
{
public:
	Uart(USART_TypeDef* usartx);
	void init(void);
	void USART1_Init(void);
	void write(char* p, uint16_t len);
	void write(char p);
	void cls(void);
	void bkspc(void);

private:
	USART_TypeDef* _usart;

};

Uart::Uart(USART_TypeDef* usartx)
	: _usart(usartx)
{}

void Uart::init(void)
{
	USART1_Init();
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


/*****************************************************
 * Initialize USART1: enable interrupt on reception
 * of a character
 *****************************************************/
void Uart::USART1_Init(void)
{
    /* Enable USART1 global interrupt */
	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_StructInit(&NVIC_InitStructure); // don't exisist

	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;

#if defined (STM32F051x8) || defined (STM32F030)
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
#else
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
#endif
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

//    // Configure Tx/Rx pins as USART1 alternate function
//    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
//    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

    // PC6=Tx PC7=Rx
#if defined (STM32F051x8) || defined (STM32F030)
	Gpio pB6 = Gpio(GPIOA,9);
	Gpio pB7 = Gpio(GPIOA, 10);
    pB6.initAFPP();
    pB7.initAFPP();
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);


#else
    Gpio pA9 = Gpio(GPIOA, 9); // Tx
    pA9.initAFPP();
    Gpio pA10 = Gpio(GPIOA, 10); // Rx
    pA10.initInFloating();
#endif

    /* Enable clock for USART1 and GPIOA */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* USART configuration structure for USART1 */
    USART_InitTypeDef usart_initStruct;
    USART_StructInit(&usart_initStruct);

        usart_initStruct.USART_BaudRate = 115200; // fast as I could get the f103 + cp2102 (supposedly 1Mbit)
    //usart_initStruct.USART_BaudRate = 460800; // fast as I could get the f103 + cp2102 (supposedly 1Mbit)
    //usart_initStruct.USART_BaudRate = 460800; // fast as I could get the f103 + cp2102 (supposedly 1Mbit)
//        usart_initStruct.USART_BaudRate = 57600;
    //    usart_initStruct.USART_BaudRate = 230400;
//    usart_initStruct.USART_BaudRate = 576000;
    usart_initStruct.USART_WordLength = USART_WordLength_8b;
    usart_initStruct.USART_StopBits = USART_StopBits_1;
    usart_initStruct.USART_Parity = USART_Parity_No;
    usart_initStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart_initStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* Configure USART1 */
    USART_Init(USART1, &usart_initStruct);

    /* Enable RXNE interrupt */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    /* Enable USART1 */
    USART_Cmd(USART1, ENABLE);
    /* Baud rate 9600, 8-bit data, One stop bit
     * No parity, Do both Rx and Tx, No HW flow control
     */
}

