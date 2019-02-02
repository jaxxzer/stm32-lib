#include "spi.h"

void Spi::init(uint32_t prescaler = SPI_BaudRatePrescaler_64, uint32_t mode = SPI_Mode_Master) {
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

  rxBuf[_rxTail] = SPI_I2S_ReceiveData(SPI2);

  while (_rxTail < len) {
    write(0x0);
    while (!SPI_I2S_GetFlagStatus(_peripheral, SPI_I2S_FLAG_RXNE))
      ;

    rxBuf[_rxTail++] = SPI_I2S_ReceiveData(SPI2);
  }
  return len;
}

void Spi::write(char d) {
  while (!SPI_I2S_GetFlagStatus(_peripheral, SPI_I2S_FLAG_TXE))
    ;
  SPI_I2S_SendData(_peripheral, d);
  while (!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
    ;
}

void Spi::write(char *d, uint16_t len) {
  while (len--) {
    write(*d++);
  }
};
