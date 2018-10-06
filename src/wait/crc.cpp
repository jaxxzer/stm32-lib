#include "crc.h"
#include "helpers.h"
#include "printing.h"
#include <stdio.h>

void crcInit()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	CRC->CR |= CRC_CR_RESET;
}

uint32_t crcCalcChecksum(uint32_t* buf, uint8_t len)
{
	CRC->CR |= CRC_CR_RESET;
	for (uint8_t i = 0; i < len; i++) {
		//print("\n\rcs byte: "); printHex(buf[i]);
		  CRC->DR = buf[i];
	}
	return CRC->DR;
}

void crcTest(void)
{
	uint32_t n = 0x77777777;
	CRC_ReverseInputDataSelect(CRC_ReverseInputData_No);
	CRC_ReverseOutputDataCmd(DISABLE);

	println(); printHex(crcCalcChecksum(&n, 1));
	CRC_ReverseInputDataSelect(CRC_ReverseInputData_32bits);
	println(); printHex(crcCalcChecksum(&n, 1));
	CRC_ReverseOutputDataCmd(ENABLE);
	println(); printHex(crcCalcChecksum(&n, 1));
	CRC_ReverseInputDataSelect(CRC_ReverseInputData_No);
	println(); printHex(crcCalcChecksum(&n, 1));
}
