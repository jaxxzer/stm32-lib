//#include "stm32lib-conf.h"
#include "gpio.h"
#include "scheduling.h"
#include "spi.h"
#include "sx1276.h"

#define USART_BAUDRATE 1e6

Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };

Gpio gpioReset { GPIO_NRST_PORT, GPIO_NRST_PIN };
    Gpio gpioNss = { GPIOB, 12 };

Spi spi;
uint8_t regValues[] = {
    0x0,
    0x9,
    0x1a,
    0xb,
    0x0,
    0x52,
    0x6c,
    0x80,
    0x0,
    0x4f,

};

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

#if defined (USE_USART_1)
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
	gpioUsart1Rx.init(GPIO_Mode_AF, GPIO_PuPd_UP);
    gpioUsart1Tx.init(GPIO_Mode_AF, GPIO_PuPd_UP);
    gpioUsart1Rx.configAF(GPIO_USART1_RX_AF);
    gpioUsart1Tx.configAF(GPIO_USART1_TX_AF);
#elif defined(STM32F1)
	gpioUsart1Rx.init(GPIO_Mode_AF_PP);
    gpioUsart1Tx.init(GPIO_Mode_AF_PP);
    Gpio::remapConfig(GPIO_USART1_REMAP, ENABLE);
#else
 #error
#endif

    uart1.init(USART_BAUDRATE);
    uart1.ITConfig(USART_IT_RXNE, ENABLE);
    uart1.setEnabled(ENABLE);
    uart1.cls();
}
#endif

#if defined (USE_USART_2)
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
#endif

#if defined(USE_USART_3)
void initUsart3(void)
{
#if defined(STM32F0)
    nvic_config(USART3_IRQn, 0, ENABLE);
#elif defined(STM32F1) || defined(STM32F3)
    nvic_config(USART3_IRQn, 0, 0, ENABLE);
#else
 #error
#endif

	gpioUsart3Rx.init(GPIO_Mode_AF);
    gpioUsart3Tx.init(GPIO_Mode_AF);

#if defined(STM32F0) || defined(STM32F3)
    gpioUsart3Rx.configAF(GPIO_USART3_RX_AF);
    gpioUsart3Tx.configAF(GPIO_USART3_TX_AF);
#elif defined(STM32F1)
    Gpio::remapConfig(GPIO_USART3_REMAP, ENABLE);
#else
 #error
#endif

    uart3.init(USART_BAUDRATE);
    uart3.ITConfig(USART_IT_RXNE, ENABLE);
    uart3.setEnabled(ENABLE);
    uart3.cls();
}
#endif

void initGpio()
{
    gpioLed.init(GPIO_Mode_Out_PP);
    Gpio gpioMosi = { GPIOB, 15 };
    Gpio gpioMiso = { GPIOB, 14 };
    Gpio gpioSck = { GPIOB, 13 };

    gpioNss.init(GPIO_Mode_Out_PP);
    gpioMosi.init(GPIO_Mode_AF_PP);
    gpioMiso.init(GPIO_Mode_IN_FLOATING);
    gpioSck.init(GPIO_Mode_AF_PP);
            gpioNss.set();

    gpioReset.init(GPIO_Mode_Out_PP);
    gpioReset.set();
}
void resetDev()
{
    gpioReset.reset();
    mDelay(10);
    gpioReset.set();
    mDelay(10);
}

char readAddr(uint8_t addr)
{
    char ch;
    spi.write((char*)&addr, 1);
    while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
    while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE));
    ch = SPI_I2S_ReceiveData(SPI2);

    spi.write((uint8_t)0);
    while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
    while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE));
    ch = SPI_I2S_ReceiveData(SPI2);

    return ch;
}

SX1276 sx1276(spi, gpioNss, gpioReset);

int main(void)
{
	configureClocks(1000);

    initUsart1();
#if defined(STM32F0) || defined(STM32F3)
    gpioLed.init(GPIO_Mode_OUT);
#elif defined(STM32F1)
    gpioLed.init(GPIO_Mode_Out_PP);
#else
 #error
#endif
    mDelay(10);
    char* str = "abcd";
    initGpio();

    spi.init(SPI_BaudRatePrescaler_16);
     SPI_SSOutputCmd(SPI2, DISABLE);

    spi.enable(ENABLE);
    uint8_t idx = 0;

    uint8_t reg = 0x1;
    uint8_t writeFlag = 0x80;
    uint8_t transferVal = 0x0;
    uint8_t a = 0;
    printf("hello\r\n");
    mDelay(1000);
    while (1) {
        transferVal = reg;
        //spi.write(&transferVal, 1);
        //spi.write(&str[idx++], 1);
        //spi.write("hellolll", 8);
        resetDev();
        // spi.init(SPI_BaudRatePrescaler_16);

        sx1276.init();

            // sx1276.dumpRegs();
            // while(1);
        mDelay(10);

        while(1) {
            // mDelay(20);
            sx1276.transmit("hello", 5);
            //sx1276.transmit();
            // sx1276.dumpRegs();
            // //while(1);
            // mDelay(100);
            gpioLed.toggle();

        }
    }

    return 0;
}
