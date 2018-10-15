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
	return ((uint8_t)(txTail - txHead) % bufSize);
}

uint8_t Uart::txSpaceAvailable(void)
{
	return bufSize - txSpaceUsed() - 1;
}

void Uart::write(const char* ch) {

	// Use this instead for blocking write
//    USART_SendData(USART1, *ch);
//    while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));

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
	write("[2J");
	//printf("[2J");    // clear screen command
	write((char*)(&esc));
	write("[H");
	//printf("[H"); // goto home
}

void Uart::bkspc(void)
{
	static const uint8_t bkspc = 0x08;
	write((char*)(&bkspc));       // ESC command
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

#ifdef USE_USART_1
Uart uart1 { USART1 };
#endif
#ifdef USE_USART_2
Uart uart2 { USART2 };
#endif
#ifdef USE_USART_3
Uart uart3 { USART3 };
#endif
