#include "stm32lib-conf.h"

#define USART_BAUDRATE 1e6

Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };

#if defined(USE_USART_1)
Gpio gpioUsart1Tx         { GPIO_USART1_TX_PORT, GPIO_USART1_TX_PIN };
Gpio gpioUsart1Rx         { GPIO_USART1_RX_PORT, GPIO_USART1_RX_PIN };
#endif

#if defined(USE_USART_3)
Gpio gpioUsart3Tx         { GPIO_USART3_TX_PORT, GPIO_USART3_TX_PIN };
Gpio gpioUsart3Rx         { GPIO_USART3_RX_PORT, GPIO_USART3_RX_PIN };
#endif

void initUsart1(void)
{
#if defined(STM32F0)
    nvic_config(USART1_IRQn, 0, ENABLE);
#elif defined(STM32F1) || defined(STM32F3)
    nvic_config(USART1_IRQn, 0, 0, ENABLE);
#else
 #error
#endif

	gpioUsart1Rx.init(GPIO_USART1_RX_MODE);
    gpioUsart1Tx.init(GPIO_USART1_TX_MODE);

#if defined(STM32F0) || defined(STM32F3)
    gpioUsart1Rx.configAF(GPIO_USART1_RX_AF);
    gpioUsart1Tx.configAF(GPIO_USART1_TX_AF);
#elif defined(STM32F1)
    Gpio::remapConfig(GPIO_USART1_REMAP, ENABLE);
#else
 #error
#endif

    uart1.init(USART_BAUDRATE);
    uart1.ITConfig(USART_IT_RXNE, ENABLE);
    uart1.setEnabled(ENABLE);
    uart1.cls();
}

int main()
{
    configureClocks(1000);

    gpioLed.init(GPIO_Mode_OUT);

    initUsart1();

    crcInit();

    uint32_t buf[] = { 0x00000000, 0x44474444, 0x88888888 };

    uint32_t crc = crcCalcChecksum(buf, sizeof(buf) / sizeof(uint32_t));
    while (1) {
        printf("crc: %d\r\n", crc);
        mDelay(500);
        gpioLed.toggle();
    }

    return 0;
}
