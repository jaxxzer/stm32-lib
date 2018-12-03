#include "stm32lib-conf.h"

class Spi
{
public:
    void enable(FunctionalState enabled) {
        SPI_Cmd(_peripheral, enabled);
    };
    void init(
              uint32_t prescaler = SPI_BaudRatePrescaler_64,
              uint32_t mode = SPI_Mode_Master) {
        _clockEnable();
        enable(ENABLE);
        mDelay(10);
        SPI_StructInit(&_config);
        _config.SPI_Mode = mode;
        _config.SPI_BaudRatePrescaler = prescaler;
        SPI_Init(_peripheral, &_config);
    };

    void write(uint8_t c)
    {
        write((char*)&c);
    }
    void write(char* d, uint16_t len) {
        while(len--) {
            write(d++);
        }
    };
    void write(char* d)
    {
        while(!SPI_I2S_GetFlagStatus(_peripheral, SPI_I2S_FLAG_TXE));
        SPI_I2S_SendData(_peripheral, *d);
    }
    void _clockEnable() {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    };
    SPI_TypeDef* _peripheral = SPI2;
    SPI_InitTypeDef _config;
};