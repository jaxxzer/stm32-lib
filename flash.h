#pragma once

#include <stdio.h>

class Flash
{
public:
	Flash(uint8_t pages, uint8_t blockSize)
		: _blockSize(blockSize)
		, _pages(pages)
	{}

	static const uint16_t FLASH_ERASED = 0xFFFF;

	uint16_t available(void);

	uint16_t* currentBlock(void);
	void erase(void);
	void init(void);
	void writeBlock(uint16_t* block, uint8_t len);
	void readBlock(uint16_t* block, uint8_t len);

	uint16_t totalSize(void) { return _pageSize * _pages; }


private:

	void _lock();
	void _unlock();
	void _waitOperation();

	uint16_t* pageAddress = (uint16_t*)0x08003C00; // Page 16
	uint16_t firstErasedOffset;
	const uint16_t _pageSize = 1024; // bytes, platform dependent 'low, medium, high density..'
	const uint8_t _pages;
	const uint16_t _blockSize;

};



void Flash::init() {
	firstErasedOffset = 0;

	for (uint16_t i = 0; i < _pageSize/2; i++) {
		uint16_t* addr = (pageAddress + i);
		uint16_t val = *addr;
		//printf("\n\rSearching for first address: %d \t%d", addr, val);
		if (val == 0xFFFF) {
			firstErasedOffset = i * sizeof(uint16_t);
			break;
		}
	}
	printf("\n\rFirst Erased: %d \tSpace remaining: %d", (uint32_t)pageAddress + firstErasedOffset, available());
	if (!firstErasedOffset) erase();
}


void Flash::erase() {
	printf("Erasing flash");
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
	printf("\n\rreading config from: %d", currentBlock());

	for (int i = 0; i < len; i++) { // TODO memcpy?
		block[i] = currentBlock()[i];
	}
}





void Flash::writeBlock(uint16_t* block, uint8_t len) {

	if (available()/2 < len) {
		erase();
	}
	for (uint8_t i = 0; i < len; i++)
	{
	       FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR);

	       while(FLASH_GetStatus() == FLASH_BUSY);
	       uint16_t offset = i * sizeof(uint16_t);
		uint32_t addr = (uint32_t)pageAddress + firstErasedOffset + offset;
		//printf("\n\r%dWriting to address: %d: %d ", i, addr, config[i]);
		FLASH_ProgramHalfWord(addr, *(block + i));
		//mDelay(50);
	}

	firstErasedOffset += len * sizeof(uint16_t);
	printf("\n\rFirst Erased: %d \tSpace remaining: %d", (uint32_t)pageAddress + firstErasedOffset, available());

	//printf("\n\rFirstErased: %d", (uint32_t)pageAddress + firstErasedOffset);
}

