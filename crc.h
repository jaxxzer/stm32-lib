#pragma once

#include "helpers.h"

uint32_t crcCalcChecksum(uint32_t* buf, uint8_t len) {
	CRC_ResetDR();
	for (uint8_t i = 0; i < len; i++) {
		CRC_CalcCRC(buf[i]);
	}
	return CRC_GetCRC();
}
void crcInit()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
//	CRC_ReverseInputDataSelect(CRC_ReverseInputData_8bits);
//	CRC_ReverseOutputDataCmd(ENABLE);
	CRC_ResetDR();

//	CRC_SetInitRegister(0);
}

void crcTest() {
	uint32_t c1 = 0x235a3ff3;
	uint32_t c2 = 0x729bce2a;
//	uint32_t c3 = 0x729bce2a;
	CRC_CalcCRC(c1);
	CRC_CalcCRC(c2);
	print("\n\n\n\rCRC: "); printHex(CRC_GetCRC());
}
