#pragma once

#include <stdio.h>

#include "helpers.h"
// On STM32F0 Datasheet:
// 16-bit programming time: 53.5 microseconds
// Page/Mass erase time: 30 milliseconds
// Endurance: 1 kcycle! *based on characterization, not empirical results

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

void Flash::printContents()
{
	print("\n\r               ");
	for (uint8_t i = 0; i < _blockSize; i++) {
		print("  ");
		my_printInt(i);
	}
	for (uint16_t* i = pageAddress; i < pageAddress + (_pageSize/2) * _pages; i += _blockSize )
	{
		printBlock(i);
	}
}

void Flash::printBlock(uint16_t* block)
{
	print("\n\r");
	for (uint16_t* i = block; i < block + _blockSize; i++) {
		print("  ");
		printHex(*i);
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
uint16_t Flash::available(void) {
	if (firstErasedOffset > _pageSize) {
		return 0;
	}
	return _pageSize - firstErasedOffset;
}



uint16_t* Flash::currentBlock(void) {
	uint16_t offset = firstErasedOffset >= _blockSize
			? firstErasedOffset - _blockSize * sizeof(uint16_t)
			: 0;

	uint32_t addr = (uint32_t)pageAddress + offset;
	return (uint16_t*)addr;
}

void Flash::readBlock(uint16_t* block, uint8_t len) {
	//printf("\n\rreading config from: %d", currentBlock());

	for (int i = 0; i < len; i++) { // TODO memcpy?
		block[i] = currentBlock()[i];
	}
}





void Flash::writeBlock(uint16_t* block, uint8_t len) {
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
		FLASH_ProgramHalfWord(addr, *(block + i));
	       while(FLASH_GetStatus() == FLASH_BUSY);

		//mDelay(50);
	}

	firstErasedOffset += len * sizeof(uint16_t);
    FLASH_Lock();

	printContents();

	//printf("\n\rFirst Erased: %d \tSpace remaining: %d", (uint32_t)pageAddress + firstErasedOffset, available());

	//printf("\n\rFirstErased: %d", (uint32_t)pageAddress + firstErasedOffset);
}

