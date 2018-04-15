#pragma once

void crcInit()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	CRC->CR |= CRC_CR_RESET;
}

uint32_t crcCalcChecksum(uint32_t* buf, uint8_t len) {
	CRC->CR |= CRC_CR_RESET;
	for (uint8_t i = 0; i < len; i++) {
		  CRC->DR = buf[i];
	}
	return CRC->DR;
}
