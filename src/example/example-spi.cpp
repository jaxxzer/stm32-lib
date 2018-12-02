//#include "stm32lib-conf.h"
#include "gpio.h"
#include "scheduling.h"
#include "spi.h"
Gpio gpioLed { GPIO_LED1_PORT, GPIO_LED1_PIN };

void initGpio()
{
    Gpio gpioNss = { GPIOB, 12 };
    Gpio gpioMosi = { GPIOB, 15 };
    Gpio gpioMiso = { GPIOB, 14 };
    Gpio gpioSck = { GPIOB, 13 };

    gpioNss.init(GPIO_Mode_AF_PP);
    gpioMosi.init(GPIO_Mode_AF_PP);
    gpioMiso.init(GPIO_Mode_IN_FLOATING);
    gpioSck.init(GPIO_Mode_AF_PP);
    
}
int main(void)
{
	configureClocks(1000);

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
    Spi spi = Spi();
    spi.init();
    SPI_SSOutputCmd(SPI2, ENABLE);

    spi.enable(ENABLE);
    uint8_t idx = 0;

    uint8_t reg = 0x1;
    uint8_t writeFlag = 0x80;
    uint8_t transferVal = 0x0;
            uint8_t a = 0;

    while (1) {
        transferVal = reg;
        //spi.write(&transferVal, 1);
        //spi.write(&str[idx++], 1);
        // spi.write("hellolll", 8);

        for (uint8_t i = 1; i < 0xa; i++)
        {
    spi.enable(ENABLE);
            spi.write((char*)&i, 1);
            spi.write((char*)&a, 1);
            while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
    //SPI_SSOutputCmd(SPI2, DISABLE);
                SPI_I2S_ReceiveData(SPI2);

    spi.enable(DISABLE);


            mDelay(1);

        }
        idx = idx %4;
        gpioLed.toggle();
        mDelay(10);
    }

    return 0;
}
