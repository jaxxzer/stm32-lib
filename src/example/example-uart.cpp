#include "stm32lib-conf.h"


#define GPIO_USART1_TX      GPIOA
#define PIN_USART1_TX       9

#define GPIO_USART1_RX      GPIOA
#define PIN_USART1_RX       10

#ifdef STM32F0
#define GPIO_LED_PORT       GPIOB
#define GPIO_LED_PIN        1
#elif STM32F1
#define GPIO_LED_PORT       GPIOB
#define GPIO_LED_PIN        13
#endif

Gpio gpioLed { GPIO_LED_PORT, GPIO_LED_PIN };

Gpio gpioUsart1Tx         { GPIO_USART1_TX, PIN_USART1_TX };
Gpio gpioUsart1Rx         { GPIO_USART1_RX, PIN_USART1_RX };

#ifdef STM32F1
#define GPIO_USART3_TX      GPIOB
#define PIN_USART3_TX       10

#define GPIO_USART3_RX      GPIOB
#define PIN_USART3_RX       11

Gpio gpioUsart3Tx         { GPIO_USART3_TX, PIN_USART3_TX };
Gpio gpioUsart3Rx         { GPIO_USART3_RX, PIN_USART3_RX };
#endif

void initUsart1(void)
{
#ifdef STM32F0
	gpioUsart1Rx.init(GPIO_Mode_AF);
    gpioUsart1Tx.init(GPIO_Mode_AF);
    gpioUsart1Rx.configAF(1);
    gpioUsart1Tx.configAF(1);
    nvic_config(USART1_IRQn, 0, ENABLE);
#elif STM32F1
	gpioUsart1Rx.init(GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
    gpioUsart1Tx.init(GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    nvic_config(USART1_IRQn, 0, 0, ENABLE);
#endif
    uart1.init(115200);
    uart1.ITConfig(USART_IT_RXNE, ENABLE);
    uart1.setEnabled(ENABLE);
    uart1.cls();
}

void initUsart3(void)
{
#ifdef STM32F1
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

void initGpioLed(void)
{
#ifdef STM32F0
    gpioLed.init(GPIO_Mode_OUT);
#elif STM32F1
    gpioLed.init(GPIO_Mode_Out_PP);
#endif
}

int main()
{
    configureClocks(1000);

    initGpioLed();
    initUsart1();
#ifdef STM32F1
    initUsart3();
#endif

    while (1) {
        printf("Initializing Wraith32\n");
        uart1.write("hello", 5);
#ifdef STM32F1
        printf("Initializing Wraith32");
        uart1.write("hello", 5);
        uart3.write("hello", 5);
#endif
        mDelay(100);
        gpioLed.toggle();
    }

    return 0;
}
