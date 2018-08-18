#include "flash.h"
#include <stdio.h>
#include "helpers.h"
#include "printing.h"

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

// Find and set firstAvailableWordOffset
void Flash::init() {
	firstAvailableWordOffset = _pageSize * _pages;

	for (uint16_t* addr = pageAddress; addr < pageAddress + _pageSize * _pages; addr++) {
		uint16_t val = *addr;
		if (val == 0xFFFF) {
			firstAvailableWordOffset = addr - pageAddress;
			break;
		}
	}
	while(firstAvailableWordOffset % 2) { // allow 32-bit alignment only, required for use with CRC peripheral
		FLASH_Unlock();
		FLASH_ProgramHalfWord((uint32_t)&pageAddress[firstAvailableWordOffset++], 0);
		FLASH_Lock();
	}
}

void Flash::erase() {
	FLASH_Unlock();
	for (uint8_t i = 0; i < _pages; i++) {
	    FLASH_ClearFlag(FLASH_FLAG_PGERR); // TODO check this
		FLASH_ErasePage((uint32_t)pageAddress + i * _pageSize);
	}
	FLASH_Lock();
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
	uint32_t* start = (uint32_t*)currentBlock();
	uint32_t cs = crcCalcChecksum(start, _blockSize/2 -1); // two 16 bit words at a time

	// blocksize is in (16bit)words
	// we are checking 32bits at a time

	// Checksum is always adjusted to prevent writing sentinel value to flash
	if ((cs & FLASH_ERASED) == FLASH_ERASED) {
		cs -= 1;
	}

	if ((cs & (FLASH_ERASED << 8)) == (FLASH_ERASED << 8)) {
		cs -= 0x10000;
	}

	return cs == start[_blockSize/2 -1];
}

void Flash::readBlock(uint16_t* block, uint8_t len)
{
	for (int i = 0; i < _blockSize; i++) { // TODO memcpy?
		block[i] = currentBlock()[i];
	}
}

// TODO use buffer/dma
void Flash::writeBlock(uint16_t* block, uint8_t len) // todo takeout len argument, use blocksize
{
	if (available() < len) {
		erase();
	}

    FLASH_Unlock();

	for (uint16_t* addr = block; addr < block + len; addr++)
	{
	    FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR); // TODO check this

		if (*addr == FLASH_ERASED) *addr = FLASH_ERASED - 1; // sentinal value not allowed
		FLASH_ProgramHalfWord((uint32_t)&pageAddress[firstAvailableWordOffset++], *addr);
	    while(FLASH_GetStatus() == FLASH_BUSY)
	    {}
	}

    FLASH_Lock();

	printContents();
}
