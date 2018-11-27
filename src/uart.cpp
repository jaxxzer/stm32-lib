#include "uart.h"

void Uart::setClockEnabled(FunctionalState enabled)
{
	switch((uint32_t)_peripheral) {
	case USART1_BASE:
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, enabled);
		break;
	case USART2_BASE:
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, enabled);
		break;
	case USART3_BASE:
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, enabled);
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
	//return ((uint8_t)(txTail - txHead) % bufSize);

	if (txTail >= txHead)
	{
		return txTail - txHead;
	} else {
		return bufSize - (txHead - txTail);
	}
}

uint8_t Uart::txSpaceAvailable(void)
{
	return bufSize - txSpaceUsed() - 1;
}

void Uart::dmaTCcallback()
{
	//dma1c7.setEnabled(DISABLE);
	    DMA_Cmd(DMA1_Channel7, DISABLE);

	txHead += _dmaTransferCount;
	txHead = txHead % bufSize;
	_dmaTransferCount = txSpaceUsed();

	if (_dmaTransferCount)
	{
		DMA1_Channel7->CNDTR = _dmaTransferCount;
		DMA1_Channel7->CMAR = (uint32_t)&txBuf[txHead];
		//dma1c7.setEnabled(ENABLE);
		    DMA_Cmd(DMA1_Channel7, ENABLE);

	}
	// TODO wtf is difference between clearflag and clearit in spl?
	DMA_ClearFlag(DMA1_FLAG_TC7);
}

void Uart::write(const char* ch) {

	// Use this instead for blocking write
//    USART_SendData(USART1, *ch);
//    while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	uint8_t x = txSpaceAvailable();
	while (!x) {
		txOverruns++; // block when buffer is full
		mDelay(1);
		x = txSpaceAvailable();
	}

	txBuf[txTail++] = *ch;
	txTail = txTail % bufSize;

	// _peripheral->CR1 |= USART_CR1_TXEIE;
}

void Uart::write(const char* ch, uint16_t len)
{
	for (uint16_t i = 0; i < len; i++) {
		write(ch++);
	}
	if (!_dmaTransferCount)
	{
		_dmaTransferCount = txSpaceUsed();
		DMA1_Channel7->CNDTR = _dmaTransferCount;
		DMA1_Channel7->CMAR = (uint32_t)&txBuf[txHead];
		//dma1c7.setEnabled(ENABLE);
		DMA_Cmd(DMA1_Channel7, ENABLE);
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
	write("[2J", 3);
	//printf("[2J");    // clear screen command
	write((char*)(&esc));
	write("[H", 2);
	//printf("[H"); // goto home
}

void Uart::bkspc(void)
{
	static const uint8_t bkspc = 0x08;
	write((char*)(&bkspc));       // ESC command
}
void Uart::dmaInit()
{
		

		Dma dma1c7 = Dma(DMA1_Channel7);
		dma1c7.init(
			(uint32_t)&(_peripheral->TDR),
			(uint32_t)txBuf,
			0, //bufSize
			DMA_DIR_PeripheralDST,
			DMA_PeripheralDataSize_Byte,
			DMA_MemoryDataSize_Byte,
			DMA_Mode_Normal,
			DMA_Priority_High,
			DMA_MemoryInc_Enable);
		
		
		
		//dma1c7.setEnabled(ENABLE);
		USART_DMACmd(_peripheral, USART_DMAReq_Tx, ENABLE);


	
}
void Uart::_irqHandler(void)
{
	// if (USART_GetITStatus(_peripheral, USART_IT_RXNE) != RESET) {
	// 	char rxdata = USART_ReceiveData(_peripheral); // reading the data clears the flag

	// 	rxBuf[rxTail++] = rxdata;
	// 	rxTail = rxTail % bufSize;

	// 	if (rxHead == rxTail) { // overwrite waiting buffer
	// 		rxOverruns++;
	// 		rxHead++;
	// 		rxHead = rxHead % bufSize;

	// 		//uart.rxTail++;
	// 	}
	// }

	// if (USART_GetITStatus(_peripheral, USART_IT_TXE) != RESET) {
	// 	if (txHead != txTail) { // if there is data in the buffer, send the first byte
	// 		USART_SendData(_peripheral, txBuf[txHead++]);
	// 		txHead = txHead % bufSize;
	// 	} else {
	// 		_peripheral->CR1 &= ~USART_CR1_TXEIE; // diable interrupt when buffer is empty
	// 	}
	// }

	// if (USART_GetFlagStatus(_peripheral, USART_IT_ORE) != RESET) {
	// 	USART_ClearITPendingBit(_peripheral, USART_IT_ORE);
	// }
}

extern "C" {
    void USART1_IRQHandler(void)
    {
#ifdef USE_USART_1
		uart1._irqHandler();
#endif
    }
	void USART2_IRQHandler(void)
    {
#ifdef USE_USART_2
		uart2._irqHandler();
#endif
    }
	void USART3_IRQHandler(void)
	{
#ifdef USE_USART_3
		uart3._irqHandler();
#endif
	}
}

extern "C"
{
	void DMA1_Channel7_IRQHandler(void)
	{
		uart2.dmaTCcallback();
	}

}

#ifdef USE_USART_1
Uart uart1 { USART1 };
#endif
#ifdef USE_USART_2
Uart uart2 { USART2 };
#endif
#ifdef USE_USART_3
Uart uart3 { USART3 };
#endif
