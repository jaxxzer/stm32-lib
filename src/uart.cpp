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

uint16_t Uart::txSpaceUsed(void)
{
	//return ((uint8_t)(txTail - txHead) % bufSize);

	if (txTail >= txHead)
	{
		return txTail - txHead;
	} else {
		return bufSize - (txHead - txTail);
	}
}

uint16_t Uart::txSpaceAvailable(void)
{
	return bufSize - txSpaceUsed() - 1;
}

void Uart::dmaTCcallback()
{
	//dma1c7.setEnabled(DISABLE);
	DMA_Cmd(_dmaCh, DISABLE);
	// TODO wtf is difference between clearflag and clearit in spl?
	DMA_ClearFlag(DMA1_FLAG_TC7);
	txHead += _dmaTransferCount;
	txHead = txHead % bufSize;

	startTxDmaTransfer();
}

void Uart::dmaHTcallback()
{
	DMA_ClearFlag(DMA1_FLAG_HT7);
	txHead += _dmaTransferCount / 2;
	_dmaTransferCount -= _dmaTransferCount / 2;
	txHead = txHead % bufSize;
}

void Uart::write(const char* ch) {
	// Use this instead for blocking write
	if(_dmaCh) { // dma mode
		while (!txSpaceAvailable()) {
			if (!(_dmaCh->CCR & 0x1)) {
				startTxDmaTransfer();
			}
			txOverruns++; // block when buffer is full
		}

		txBuf[txTail++] = *ch;
		txTail = txTail % bufSize;
	} else { // interrupt mode
		while (!txSpaceAvailable()) {
			txOverruns++; // block when buffer is full
		}

		txBuf[txTail++] = *ch;
		txTail = txTail % bufSize;

		_peripheral->CR1 |= USART_CR1_TXEIE;
	}
}

uint16_t Uart::dmaToTransfer()
{
	if(txTail >= txHead) {
		return txTail - txHead;
	} else {
		return bufSize - txHead;
	}
}

void Uart::startTxDmaTransfer()
{
	_dmaTransferCount = dmaToTransfer();
	if (_dmaTransferCount) {
		_dmaCh->CNDTR = _dmaTransferCount;
		_dmaCh->CMAR = (uint32_t)&txBuf[txHead];
		//dma1c7.setEnabled(ENABLE);
		DMA_Cmd(_dmaCh, ENABLE);
	}
}

void Uart::write(const char* ch, uint16_t len)
{
	for (uint16_t i = 0; i < len; i++) {
		write(ch++);
	}
	if (!(_dmaCh->CCR & 0x1)) {
		startTxDmaTransfer();
	}
}

uint16_t Uart::rxWaiting(void)
{
	return ((uint16_t)(rxTail - rxHead) % bufSize);
}

int Uart::read(void)
{
	if (!rxWaiting()) {
		return -1;
	}
	// todo read character out first
	uint16_t bufIndex = rxHead++;
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
void Uart::dmaTxInit()
{
	uint32_t dmaIRQn = 0;
#if defined(STM32F0)
	switch((uint32_t)_peripheral) {
		case USART1_BASE:
			break;
		case USART2_BASE:
			_dmaCh = DMA1_Channel5;
			dmaIRQn = DMA1_Channel4_5_IRQn;
			break;
		case USART3_BASE:
			break;
	}
#elif defined(STM32F1)
	switch((uint32_t)_peripheral) {
		case USART1_BASE:
			break;
		case USART2_BASE:
			_dmaCh = DMA1_Channel7;
			dmaIRQn = DMA1_Channel7_IRQn;
			break;
		case USART3_BASE:
			break;
	}
#elif defined(STM32F3)
#else
 #error
#endif

	if (!_dmaCh) {
		return;
	}

	Dma dma1c7 = Dma(_dmaCh);
	dma1c7.init(
#if defined(STM32F0)
		(uint32_t)&(_peripheral->TDR),
#elif defined(STM32F1)
		(uint32_t)&(_peripheral->DR),
#elif defined(STM32F3)
		(uint32_t)&(_peripheral->TDR),
#else
#error
#endif
		(uint32_t)txBuf,
		0, //bufSize (CNDTR)
		DMA_DIR_PeripheralDST,
		DMA_PeripheralDataSize_Byte,
		DMA_MemoryDataSize_Byte,
		DMA_Mode_Normal,
		DMA_Priority_High,
		DMA_MemoryInc_Enable);
	
	
	
	//dma1c7.setEnabled(ENABLE);
	USART_DMACmd(_peripheral, USART_DMAReq_Tx, ENABLE);

	DMA_ITConfig(_dmaCh, DMA_IT_TC, ENABLE);
    //DMA_ITConfig(_dmaCh, DMA_IT_HT, ENABLE);
#if defined(STM32F0)
    nvic_config(dmaIRQn, 0, ENABLE);
#elif defined(STM32F1) || defined(STM32F3)
    nvic_config(dmaIRQn, 0, 0, ENABLE);
#endif
}

void Uart::_irqHandler(void)
{
	if (USART_GetITStatus(_peripheral, USART_IT_RXNE) != RESET) {
		char rxdata = USART_ReceiveData(_peripheral); // reading the data clears the flag

		rxBuf[rxTail++] = rxdata;
		rxTail = rxTail % bufSize;

		if (rxHead == rxTail) { // overwrite waiting buffer
			rxOverruns++;
			rxHead++;
			rxHead = rxHead % bufSize;

			//uart.rxTail++;
		}
	}

	if (USART_GetITStatus(_peripheral, USART_IT_TXE) != RESET) {
		if (txHead != txTail) { // if there is data in the buffer, send the first byte
			USART_SendData(_peripheral, txBuf[txHead++]);
			txHead = txHead % bufSize;
		} else {
			_peripheral->CR1 &= ~USART_CR1_TXEIE; // diable interrupt when buffer is empty
		}
	}

	if (USART_GetFlagStatus(_peripheral, USART_IT_ORE) != RESET) {
		USART_ClearITPendingBit(_peripheral, USART_IT_ORE);
	}
}

extern "C" {
#ifdef USE_USART_1

    void USART1_IRQHandler(void)
    {
		uart1._irqHandler();
    }
#endif

#ifdef USE_USART_2

	void USART2_IRQHandler(void)
    {
		uart2._irqHandler();
    }
	void DMA1_Channel7_IRQHandler(void)
	{
		if (DMA_GetITStatus(DMA1_IT_HT7) != RESET) {
		uart2.dmaHTcallback();

		}
		if (DMA_GetITStatus(DMA1_IT_TC7) != RESET) {
		uart2.dmaTCcallback();

		}
	}
	
#endif

#ifdef USE_USART_3

	void USART3_IRQHandler(void)
	{
		uart3._irqHandler();
	}
#endif

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
