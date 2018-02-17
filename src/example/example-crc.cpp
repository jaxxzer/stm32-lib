#include "stm32lib-conf.h"

#define USART_BAUDRATE 1e6

Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };

#if defined(USE_USART_1)
Gpio gpioUsart1Tx         { GPIO_USART1_TX_PORT, GPIO_USART1_TX_PIN };
Gpio gpioUsart1Rx         { GPIO_USART1_RX_PORT, GPIO_USART1_RX_PIN };
#endif

#if defined(USE_USART_2)
Gpio gpioUsart2Tx         { GPIO_USART2_TX_PORT, GPIO_USART2_TX_PIN };
Gpio gpioUsart2Rx         { GPIO_USART2_RX_PORT, GPIO_USART2_RX_PIN };
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

	gpioUsart1Rx.init(GPIO_Mode_AF);
    gpioUsart1Tx.init(GPIO_Mode_AF);

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

void initUsart2(void)
{
#if defined(STM32F0)
    nvic_config(USART2_IRQn, 0, ENABLE);
#elif defined(STM32F1) || defined(STM32F3)
    nvic_config(USART2_IRQn, 0, 0, ENABLE);
#else
 #error
#endif

	gpioUsart2Rx.init(GPIO_Mode_AF);
    gpioUsart2Tx.init(GPIO_Mode_AF);

#if defined(STM32F0) || defined(STM32F3)
    gpioUsart2Rx.configAF(GPIO_USART2_RX_AF);
    gpioUsart2Tx.configAF(GPIO_USART2_TX_AF);
#elif defined(STM32F1)
    Gpio::remapConfig(GPIO_USART2_REMAP, ENABLE);
#else
 #error
#endif

    uart2.init(USART_BAUDRATE);
    uart2.ITConfig(USART_IT_RXNE, ENABLE);
    uart2.setEnabled(ENABLE);
    uart2.cls();
}

int main()
{
    configureClocks(1000);

    gpioLed.init(GPIO_Mode_OUT);

    initUsart2();

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
