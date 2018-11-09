#pragma once

#include <stdio.h>
#include "crc.h"
#include "helpers.h"

// STM32F0 Datasheet:
// 16-bit programming time: 53.5 microseconds
// Page/Mass erase time: 30 milliseconds
// Endurance: 1 kcycle only! *based on characterization, not empirical results

// _blockSize in 16bit words

// TODO make singleton/static
class Flash
{
public:
	Flash(uint8_t pages, uint8_t blockSize)
		: _blockSize(blockSize) // add two 16 bit words for the 32 bit checksum
		, _pages(pages)
	{
	    FLASH_SetLatency(FLASH_Latency_1);
	    crcInit();
	}

	static const uint16_t FLASH_ERASED = 0xFFFF;

	uint16_t available(void);
	uint16_t* currentBlock(void);

	// Initialize
	void init(void);
	void erase(void); // Erase entire reserved flash region
	void writeBlock(uint16_t* block, uint8_t len);
	void readBlock(uint16_t* block, uint8_t len);
	bool verifyChecksum(void);

	uint16_t totalSize(void) { return _pageSize * _pages; }

	void printBlock(uint16_t* block);
	void printContents();

private:

	void _lock(); // Write lock flash
	void _unlock(); // Unlock flash for write access
	void _waitOperation(); // Wait for current flash operation to complete

	const uint8_t wordSize = 2; // 16 bit word, 2 bytes

	uint16_t* pageAddress = (uint16_t*)0x08003C00; // Page 16 // TODO set this right
	uint16_t firstAvailableWordOffset;
	const uint16_t _pageSize = 1024 / sizeof(uint16_t); // platform dependent 'low, medium, high density..'
	const uint8_t _pages;
	const uint16_t _blockSize; // words
};
