#pragma once

#include <inttypes.h>
void crcInit();

uint32_t crcCalcChecksum(uint32_t *buf, uint8_t len);

void crcTest(void);
