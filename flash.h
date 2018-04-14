#pragma once

#include <stdio.h>

#include "helpers.h"
// On STM32F0 Datasheet:
// 16-bit programming time: 53.5 microseconds
// Page/Mass erase time: 30 milliseconds
// Endurance: 1 kcycle! *based on characterization, not empirical results
uint32_t crcCalcChecksum(uint32_t* buf, uint8_t len) {
	CRC_ResetDR();
	for (uint8_t i = 0; i < len; i++) {
		CRC_CalcCRC(buf[i]);
	}
	//return CRC_GetCRC();
	return 0xFFFFFFFF;
}

class Flash
{
public:
	Flash(uint8_t pages, uint8_t blockSize)
		: _blockSize(blockSize)
		, _pages(pages)
	{
	    FLASH_SetLatency(FLASH_Latency_1);
	}

	static const uint16_t FLASH_ERASED = 0xFFFF;

	uint16_t available(void);

	uint16_t* currentBlock(void);
	void erase(void);
	void init(void);
	void writeBlock(uint16_t* block, uint8_t len);
	void readBlock(uint16_t* block, uint8_t len);
	bool verifyChecksum(void);

	uint16_t totalSize(void) { return _pageSize * _pages; }

	void printContents();

private:

	void _lock();
	void _unlock();
	void _waitOperation();

	const uint8_t wordSize = 2;

	uint16_t* pageAddress = (uint16_t*)0x08003C00; // Page 16
	uint16_t firstErasedOffset;
	const uint16_t _pageSize = 1024; // TODO words, also platform dependent 'low, medium, high density..'
	const uint8_t _pages;
	const uint16_t _blockSize;

	void printBlock(uint16_t* block);

};
uint8_t bNum;
void Flash::printContents()
{
	println();
	print("    ");
	for (uint8_t i = 0; i < _blockSize; i++) {
		print("       ");
		my_printInt(i);
	}
	bNum = 0;

	for (uint16_t* i = pageAddress; i < pageAddress + (_pageSize/2) * _pages; i += _blockSize )
	{
		printBlock(i);
	}
}

void Flash::printBlock(uint16_t* block)
{
	println();
	my_printInt(bNum++);
	if ((bNum) < 11) { // 0-9...
		print(" ");
	}
	print("   ");
	for (uint16_t* i = block; i < block + _blockSize; i++) {
		if (i < pageAddress + (_pageSize/2) * _pages) {
			print(" ");
			printHex(*i);
		}
	}
}

void Flash::init() {
	firstErasedOffset = _pageSize;

	for (uint16_t i = 0; i < _pageSize/2; i++) {
		uint16_t* addr = (pageAddress + i);
		uint16_t val = *addr;
		//printf("\n\rSearching for first address: %d \t%d", addr, val);
		if (val == 0xFFFF) {
			firstErasedOffset = i * sizeof(uint16_t);
			break;
		}
	}
	//printf("\n\rFirst Erased: %d \tSpace remaining: %d", (uint32_t)pageAddress + firstErasedOffset, available());
}


void Flash::erase() {
	//printf("Erasing flash");
	FLASH_ErasePage((uint32_t)pageAddress);
	firstErasedOffset = 0;
}

// bytes remaining
uint16_t Flash::available(void)
{
	if (firstErasedOffset > _pageSize) {
		return 0;
	}
	return _pageSize - firstErasedOffset;
}



uint16_t* Flash::currentBlock(void)
{
	uint16_t offset = firstErasedOffset >= _blockSize
			? firstErasedOffset - _blockSize * sizeof(uint16_t)
			: 0;

	uint32_t addr = (uint32_t)pageAddress + offset;
	return (uint16_t*)addr;
}

bool Flash::verifyChecksum(void)
{
	print("\n\n\n\n");

	uint32_t* start = (uint32_t*)currentBlock();
	uint32_t cs = crcCalcChecksum(start, _blockSize/2 -1);
	// blocksize is in (16bit)words
	// we are checking 32bits at a time
	print("\n\rChecksum: "); printHex(cs); print("Check: "); printHex(start[_blockSize/2 -1]);

	if ((cs & FLASH_ERASED) == FLASH_ERASED) {
		cs -= 1;
	}

	if ((cs & (FLASH_ERASED << 8)) == (FLASH_ERASED << 8)) {
		cs -= 0x10000;
	}

	print("\n\rChecksum: "); printHex(cs); print("Check: "); printHex(start[_blockSize/2 -1]);

	return cs == start[_blockSize/2 -1];
}



void Flash::readBlock(uint16_t* block, uint8_t len)
{
	//printf("\n\rreading config from: %d", currentBlock());

	for (int i = 0; i < len; i++) { // TODO memcpy?
		block[i] = currentBlock()[i];
	}
}





void Flash::writeBlock(uint16_t* block, uint8_t len)
{
    FLASH_Unlock();
	if (available()/2 < len) {
		erase();
	}
	for (uint8_t i = 0; i < len; i++)
	{
	       FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR);

	       uint16_t offset = i * sizeof(uint16_t);
		uint32_t addr = (uint32_t)pageAddress + firstErasedOffset + offset;
		//printf("\n\r%dWriting to address: %d: %d ", i, addr, config[i]);
		uint16_t value = *(block + i);

		if (value == FLASH_ERASED) value = FLASH_ERASED - 1; // sentinal value not allowed
		FLASH_ProgramHalfWord(addr, value);
	    while(FLASH_GetStatus() == FLASH_BUSY)
	    {}
	}

	firstErasedOffset += len * sizeof(uint16_t);
    FLASH_Lock();

	printContents();

	//printf("\n\rFirst Erased: %d \tSpace remaining: %d", (uint32_t)pageAddress + firstErasedOffset, available());

	//printf("\n\rFirstErased: %d", (uint32_t)pageAddress + firstErasedOffset);
}

