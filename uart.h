/*
 * uart.h
 *
 *  Created on: Feb 11, 2018
 *      Author: jack
 */

#ifndef UART_H_
#define UART_H_


class Uart
{
public:
	Uart(USART_TypeDef* usartx);
	void init(void);
	void USART1_Init(void);
	void write(char* p, uint8_t len);
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

/*****************************************************
 * Initialize USART1: enable interrupt on reception
 * of a character
 *****************************************************/
void Uart::USART1_Init(void)
{
    /* Enable USART1 global interrupt */
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
#ifdef FUCKME
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
#ifdef FUCKME
    gpio_initStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    gpio_initStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio_initStruct.GPIO_OType = GPIO_OType_PP;
    gpio_initStruct.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_initStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOA, &gpio_initStruct);
#else
    Gpio pA9 = Gpio(GPIOA, GPIO_Pin_9); // Tx
    pA9.initAFPP();
    Gpio pA10 = Gpio(GPIOA, GPIO_Pin_10); // Rx
    pA10.initInFloating();
#endif

    /* Enable clock for USART1 and GPIOA */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* USART configuration structure for USART1 */
    USART_InitTypeDef usart_initStruct;

    usart_initStruct.USART_BaudRate = 460800;
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

#ifdef __cplusplus
 extern "C" {
#endif
	/**********************************************************
	 * USART1 interrupt request handler
	 *********************************************************/
	void USART1_IRQHandler(void)
	{
		/* RXNE handler */
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
		{
			char rxdata = USART_ReceiveData(USART1);
			USART_SendData(USART1, rxdata);
		}

		/* ------------------------------------------------------------ */
		/* Other USART1 interrupts handler can go here ...             */
	}
#ifdef __cplusplus
 }
#endif
#endif /* UART_H_ */

