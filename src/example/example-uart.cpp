#include "stm32lib-conf.h"

Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };

Gpio gpioUsart1Tx         { GPIO_USART1_TX_PORT, GPIO_USART1_TX_PIN };
Gpio gpioUsart1Rx         { GPIO_USART1_RX_PORT, GPIO_USART1_RX_PIN };

#if defined(STM32F1) || defined(STM32F3)
Gpio gpioUsart3Tx         { GPIO_USART3_TX, PIN_USART3_TX };
Gpio gpioUsart3Rx         { GPIO_USART3_RX, PIN_USART3_RX };
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

#if defined(STM32F0) || defined(STM32F3)
	gpioUsart1Rx.init(GPIO_USART1_RX_MODE);
    gpioUsart1Tx.init(GPIO_USART1_TX_MODE);
    gpioUsart1Rx.configAF(GPIO_USART1_RX_AF);
    gpioUsart1Tx.configAF(GPIO_USART1_TX_AF);
    nvic_config(USART1_IRQn, 0, ENABLE);
#elif defined(STM32F1)
	gpioUsart1Rx.init(GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpioUsart1Tx.init(GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    nvic_config(USART1_IRQn, 0, 0, ENABLE);
#else
#error
#endif
    uart1.init(3000000);
    uart1.ITConfig(USART_IT_RXNE, ENABLE);
    uart1.setEnabled(ENABLE);
    uart1.cls();
}

void initUsart3(void)
{
#if defined(STM32F1)
    // TODO move to gpio class
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //mDelay(10);
    //GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
	gpioUsart3Rx.init(GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpioUsart3Tx.init(GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
    //GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    nvic_config(USART3_IRQn, 0, 0, ENABLE);
    
    uart3.init(115200);
    uart3.ITConfig(USART_IT_RXNE, ENABLE);
    uart3.setEnabled(ENABLE);
    uart3.cls();
#endif
}

int main()
{
    configureClocks(1000);

    gpioLed.init(GPIO_LED1_MODE);
    initUsart1();
#if defined(STM32F1)
    initUsart3();
#endif

    while (1) {
        printf("Initializing Wraith32\r\n");
        uart1.write("hello", 5);
#if defined(STM32F1)
        printf("Initializing Wraith32");
        uart1.write("hello", 5);
        uart3.write("hello", 5);
#endif
        //mDelay(100);
        gpioLed.toggle();
    }

    return 0;
}
