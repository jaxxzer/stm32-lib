#include "spi.h"

void Spi::init(uint32_t prescaler, uint32_t mode) {
  SPI_StructInit(&_config);
  _config.SPI_Mode = mode;
  _config.SPI_NSS = SPI_NSS_Soft;
  _config.SPI_BaudRatePrescaler = prescaler;
  SPI_Init(_peripheral, &_config);
};

uint16_t Spi::read(uint8_t base, uint16_t len) {
  write(base);
  _rxTail = 0;
  while (!SPI_I2S_GetFlagStatus(_peripheral, SPI_I2S_FLAG_RXNE))
    ;

#if defined(STM32F0) || defined(STM32F3)
  rxBuf[_rxTail] = SPI_ReceiveData8(SPI2);
#elif defined(STM32F1)
  rxBuf[_rxTail] = SPI_I2S_ReceiveData(SPI2);
#else
#error
#endif

  while (_rxTail < len) {
    write(0x0);
    while (!SPI_I2S_GetFlagStatus(_peripheral, SPI_I2S_FLAG_RXNE))
      ;

#if defined(STM32F0) || defined(STM32F3)
    rxBuf[_rxTail++] = SPI_ReceiveData8(SPI2);
#elif defined(STM32F1)
    rxBuf[_rxTail++] = SPI_I2S_ReceiveData(SPI2);
#else
#error
#endif
  }
  return len;
}

void Spi::write(char d) {
  while (!SPI_I2S_GetFlagStatus(_peripheral, SPI_I2S_FLAG_TXE))
    ;

#if defined(STM32F0) || defined(STM32F3)
  SPI_SendData8(_peripheral, d);
#elif defined(STM32F1)
  SPI_I2S_SendData(_peripheral, d);
#else
#error
#endif

  while (!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
    ;
}

void Spi::write(char *d, uint16_t len) {
  while (len--) {
    write(*d++);
  }
};
