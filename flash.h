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

	uint16_t* pageAddress = (uint16_t*)0x08003C00; // Page 16
	uint16_t firstAvailableWordOffset;
	const uint16_t _pageSize = 1024 / sizeof(uint16_t); // TODO words, also platform dependent 'low, medium, high density..'
	const uint8_t _pages;
	const uint16_t _blockSize;


};

void Flash::printContents()
{
	println();
	print("  ");
	for (uint8_t i = 0; i < _blockSize; i++) {
		print("    ");
		my_printInt(i);
	}

	uint8_t bNum = 0;
	for (uint16_t* i = pageAddress; i < pageAddress + _pageSize * _pages; i += _blockSize)
	{
		println();
		my_printInt(bNum++);
		if ((bNum) < 11) { // 0-9...
			print(" ");
		}
		print("   ");
		printBlock(i);
	}
}

void Flash::printBlock(uint16_t* block)
{
	for (uint16_t* i = block; i < block + _blockSize; i++) {
		if (i < pageAddress + _pageSize * _pages) {
			print(" ");
			printHex(*i);
		}
	}
}

void Flash::init() {

	firstAvailableWordOffset = _pageSize * _pages;

	for (uint16_t* addr = pageAddress; addr < pageAddress + _pageSize * _pages; addr++) {
		uint16_t val = *addr;
		if (val == 0xFFFF) {
			firstAvailableWordOffset = addr - pageAddress;
			break;
		}
	}

//	for (uint16_t i = 0; i < _pageSize; i++) {
//		uint16_t* addr = (pageAddress + i);
//		uint16_t val = *addr;
//		//printf("\n\rSearching for first address: %d \t%d", addr, val);
//		if (val == 0xFFFF) {
//			firstAvailableWordOffset = i * sizeof(uint16_t);
//			break;
//		}
//	}
	//printf("\n\rFirst Erased: %d \tSpace remaining: %d", (uint32_t)pageAddress + firstErasedOffset, available());
}


void Flash::erase() {
	for (uint8_t i = 0; i < _pages; i++) {
		FLASH_ErasePage((uint32_t)pageAddress + i * _pageSize);
	}
	firstAvailableWordOffset = 0;
}

// bytes remaining
uint16_t Flash::available(void)
{
	if (firstAvailableWordOffset > _pageSize) {
		return 0;
	}
	return _pageSize - firstAvailableWordOffset;
}



uint16_t* Flash::currentBlock(void)
{
	uint16_t offset = firstAvailableWordOffset >= _blockSize
			? firstAvailableWordOffset - _blockSize
			: 0;

	return pageAddress + offset;
}

bool Flash::verifyChecksum(void)
{
	print("\n\n\n\n");

	uint32_t* start = (uint32_t*)currentBlock();
	uint32_t cs = crcCalcChecksum(start, _blockSize/2 -1);
	// blocksize is in (16bit)words
	// we are checking 32bits at a time

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




// TODO use buffer/dma
void Flash::writeBlock(uint16_t* block, uint8_t len) // todo takeout len argument, use blocksize
{
    FLASH_Unlock();

	if (available() < len) {
		erase();
	}

	for (uint8_t i = 0; i < len; i++)
	{
	    FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR); // TODO check this

		uint16_t value = block[i];

		if (value == FLASH_ERASED) value = FLASH_ERASED - 1; // sentinal value not allowed
		FLASH_ProgramHalfWord((uint32_t)&pageAddress[firstAvailableWordOffset++], value);
	    while(FLASH_GetStatus() == FLASH_BUSY)
	    {}
	}

    FLASH_Lock();

	printContents();
}

