#include "uart.h"

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
	printf("[2J");    // clear screen command
	write((char*)(&esc));
	printf("[H"); // goto home
}

void Uart::bkspc(void)
{
	static const uint8_t bkspc = 0x08;
	write((char*)(&bkspc));       // ESC command
}


extern "C" {
    /**********************************************************
     * USART1 interrupt request handler
     *********************************************************/
    extern Uart uart1;
    void USART1_IRQHandler(void)
    {
        if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
            char rxdata = USART_ReceiveData(USART1); // reading the data clears the flag

            uart1.rxBuf[uart1.rxTail++] = rxdata;
            uart1.rxTail = uart1.rxTail % uart1.bufSize;

            if (uart1.rxHead == uart1.rxTail) { // overwrite waiting buffer
            	uart1.rxOverruns++;
            	uart1.rxHead++;
            	uart1.rxHead = uart1.rxHead % uart1.bufSize;

                //uart.rxTail++;
            }
        }

        if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) {
            if (uart1.txHead != uart1.txTail) { // if there is data in the buffer, send the first byte
                USART_SendData(USART1, uart1.txBuf[uart1.txHead++]);
                uart1.txHead = uart1.txHead % uart1.bufSize;
            } else {
            	uart1._peripheral->CR1 &= ~USART_CR1_TXEIE; // diable interrupt when buffer is empty
            }
        }

        if (USART_GetFlagStatus(USART1, USART_IT_ORE) != RESET) {
            USART_ClearITPendingBit(USART1, USART_IT_ORE);
        }
    }
}



void print(const char* c) {
	while (*c != 0) {
		uart1.write(c++);
	}
}

void println(void)
{
	print("\n\r");
}
void printHex(uint32_t i)
{
	uint8_t size = 4;
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
	uart1.write(&buf[p], size);
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

void print(uint16_t i) {
	char c[5];
	uint8_t len = 5;

	uint8_t p = i % 10;
	c[--len] = p + '0';
	i -= p; // not neccessary? taken care of with integer division

	while (i) {
		i /= 10;
		p = i % 10;
		c[--len] = p  + '0';
		i -= p; // not neccessary? taken care of with integer division
	}
	uart1.write(&c[len], 5-len);
}

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
	uart1.write(&c[len], 10-len);
}
