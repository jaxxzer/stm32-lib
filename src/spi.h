#pragma once
#include "stm32lib-conf.h"

class Spi {
public:
  Spi() {
    _clockEnable();
  }
  
  void _clockEnable() { RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); };
  void enable(FunctionalState enabled) { SPI_Cmd(_peripheral, enabled); };
  void init(uint32_t prescaler = SPI_BaudRatePrescaler_64, uint32_t mode = SPI_Mode_Master);
  void write(char d);
  void write(char *d, uint16_t len);
  uint16_t read(uint8_t base, uint16_t len);

  static const uint16_t rxBufSize = 256;
  uint8_t rxBuf[rxBufSize];
  uint16_t _rxTail = 0;
  
  SPI_TypeDef *_peripheral = SPI2;
  SPI_InitTypeDef _config;
};