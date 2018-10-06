#pragma once

class Dma {
	public:
	Dma(DMA_Channel_TypeDef* peripheral) : _peripheral(peripheral) {
		_clockEnable();
	};

	DMA_Channel_TypeDef* _peripheral;

	void init(	uint32_t peripheralBaseAddr,
				uint32_t memoryBaseAddr,
				uint32_t bufferSize,
				uint32_t direction = DMA_DIR_PeripheralSRC,
				uint32_t peripheralDataSize = DMA_PeripheralDataSize_Byte,
				uint32_t memoryDataSize = DMA_MemoryDataSize_Byte,
				uint32_t mode = DMA_Mode_Normal,
				uint32_t priority = DMA_Priority_Low,
				uint32_t memoryInc = DMA_MemoryInc_Disable,
				uint32_t peripheralInc = DMA_PeripheralInc_Disable,
				uint32_t m2m = DMA_M2M_Disable)
	{
		DMA_InitTypeDef _config;

		_config.DMA_PeripheralBaseAddr = peripheralBaseAddr;
		_config.DMA_MemoryBaseAddr = memoryBaseAddr;
		_config.DMA_DIR = direction;
		_config.DMA_BufferSize = bufferSize;
		_config.DMA_PeripheralInc = peripheralInc;
		_config.DMA_MemoryInc = memoryInc;
		_config.DMA_PeripheralDataSize = peripheralDataSize;
		_config.DMA_MemoryDataSize = memoryDataSize;
		_config.DMA_Mode = mode;
		_config.DMA_Priority = priority;
		_config.DMA_M2M = m2m;
		DMA_Init(_peripheral, &_config);
	}

	void setEnabled(FunctionalState enabled) {
		DMA_Cmd(_peripheral, enabled);
	}

	void _clockEnable(void) {
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE); TODO: pick
	}
};
