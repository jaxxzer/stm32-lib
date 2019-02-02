#pragma once
#include "target.h"
#include "dma.h"

#include "stm32lib-conf.h"
#include <stdio.h>

//TODO wtf do I have to forward declare this, the include is above?
class Dma;

class Uart
{
public:
	Uart(USART_TypeDef* usartx) : _peripheral(usartx) {
		setClockEnabled(ENABLE);
	}
	void write(const char* p, uint16_t len);
	void write(const char* p);

	int read(void);
	uint16_t rxWaiting(void);

	void cls(void);
	void bkspc(void);
	void ITConfig(uint32_t it, FunctionalState enabled);
	uint16_t dmaToTransfer();
	uint16_t txSpaceUsed(void);
	uint16_t txSpaceAvailable(void);
	void init(uint32_t baudrate,
				uint32_t stopbits = USART_StopBits_1,
				uint32_t wordlength = USART_WordLength_8b,
				uint32_t parity = USART_Parity_No,
				uint32_t mode = USART_Mode_Rx | USART_Mode_Tx,
				uint32_t flowcontrol = USART_HardwareFlowControl_None)
	{
		//txBuf[bufSize] = 'F';
		USART_InitTypeDef _config;
		USART_StructInit(&_config);
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
#if defined(STM32F0)
	void startAutoBaud() {
		USART_AutoBaudRateConfig(_peripheral, USART_AutoBaudRate_StartBit);
		USART_AutoBaudRateCmd(_peripheral, ENABLE);
	}
#endif

	void setEnabled(FunctionalState enabled) {
		USART_Cmd(_peripheral, enabled);
	}

	void setClockEnabled(FunctionalState enabled);
	void dmaTxInit();
	void dmaTCcallback();
	void dmaHTcallback();
	void startTxDmaTransfer();
	static const uint16_t bufSize = 512;
	char rxBuf[bufSize];
	char txBuf[bufSize];

	// tail == head      means empty
	// tail == head - 1  means full
	uint16_t rxHead = 0; // TODO allow use of full buffer, right now we use only bufSize-1 bytes
	uint16_t rxTail = 0;
	uint16_t rxOverruns = 0;
	volatile uint16_t _dmaTransferCount = 0;
	volatile uint16_t txHead;
	volatile uint16_t txTail;
	uint16_t txOverruns;

	USART_TypeDef* _peripheral;
	Dma* dmaTx;
	Dma* dmaRx;

	void _irqHandler(void);

private:
	bool _dmaMode = false; // default is interrupt-driven
	DMA_Channel_TypeDef* _dmaCh = nullptr;

};

#ifdef USE_USART_1
extern Uart uart1;
#endif
#ifdef USE_USART_2
extern Uart uart2;
#endif
#ifdef USE_USART_3
extern Uart uart3;
#endif
