#pragma once
#include "stm32lib-conf.h"

class Spi
{
public:
    Spi() 
    {
        _clockEnable();
        //mDelay(10);
    }
    void enable(FunctionalState enabled) {
        SPI_Cmd(_peripheral, enabled);
    };
    void init(
        uint32_t prescaler = SPI_BaudRatePrescaler_64,
        uint32_t mode = SPI_Mode_Master) {
        SPI_StructInit(&_config);
        _config.SPI_Mode = mode;
        _config.SPI_NSS = SPI_NSS_Soft;
        _config.SPI_BaudRatePrescaler = prescaler;
        SPI_Init(_peripheral, &_config);
    };


    void write(char* d, uint16_t len) {
        while(len--) {
            write(*d++);
        }
    };

    static const uint16_t rxBufSize = 256;
    uint8_t rxBuf[rxBufSize];
    uint16_t _rxTail = 0;
    uint16_t read(uint8_t base, uint16_t len) {
        write(base);
        _rxTail = 0;
        while(!SPI_I2S_GetFlagStatus(_peripheral, SPI_I2S_FLAG_RXNE));

        rxBuf[_rxTail] = SPI_I2S_ReceiveData(SPI2);

        while(_rxTail < len) {
            write(0x0);
            while(!SPI_I2S_GetFlagStatus(_peripheral, SPI_I2S_FLAG_RXNE));

            rxBuf[_rxTail++] = SPI_I2S_ReceiveData(SPI2);
        }
        return len;
    }
    void write(char d)
    {
        while(!SPI_I2S_GetFlagStatus(_peripheral, SPI_I2S_FLAG_TXE));
        SPI_I2S_SendData(_peripheral, d);
        while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
    }
    void _clockEnable() {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    };
    SPI_TypeDef* _peripheral = SPI2;
    SPI_InitTypeDef _config;
};