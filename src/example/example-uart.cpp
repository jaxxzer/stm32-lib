#include "stm32lib-conf.h"

#ifdef STM32F0
    Gpio gpio_Led { GPIOB, 1 };
#elif STM32F1
    Gpio gpio_Led { GPIOB, 13 };
#endif

// Callback to run on UPDATE interrupt event
void tim1UpdateCallback(void)
{
    gpio_Led.toggle();
}

void initUart(void)
{
#define GPIO_UART_TX      GPIOA
#define PIN_UART_TX       9

#define GPIO_UART_RX      GPIOA
#define PIN_UART_RX       10

	Gpio gpio_uart_tx         { GPIO_UART_TX, PIN_UART_TX };
	Gpio gpio_uart_rx         { GPIO_UART_RX, PIN_UART_RX };

#ifdef STM32F0
	gpio_uart_rx.init(GPIO_Mode_AF, GPIO_PuPd_UP);
    gpio_uart_tx.init(GPIO_Mode_AF, GPIO_PuPd_UP);
    gpio_uart_rx.configAF(0);
    gpio_uart_tx.configAF(0);
    nvic_config(USART1_IRQn, 0, ENABLE);
#elif STM32F1
	gpio_uart_rx.init(GPIO_Mode_AF_OD);
    gpio_uart_tx.init(GPIO_Mode_AF_PP);
    nvic_config(USART1_IRQn, 0, 0, ENABLE);
#endif

    uart1.ITConfig(USART_IT_RXNE, ENABLE);
    uart1.init(115200);
    uart1.setEnabled(ENABLE);
    uart1.cls();
    //uart1.startAutoBaud(); // uncomment for autobauding
}
int main()
{
    configureClocks(1000);

#ifdef STM32F0
    gpio_Led.init(GPIO_Mode_OUT);
    nvic_config(TIM1_BRK_UP_TRG_COM_IRQn, 0, ENABLE);
#elif STM32F1
    gpio_Led.init(GPIO_Mode_Out_PP);
    nvic_config(TIM1_UP_IRQn, 0, 0, ENABLE);
#endif
    initUart();

    timer1.initFreq(10); // 10Hz update
    timer1.setupUpCallback(&tim1UpdateCallback);
    timer1.setEnabled(ENABLE);
    timer1.interruptConfig(TIM_IT_Update, ENABLE);

    while (1) {  }

    return 0;
}
