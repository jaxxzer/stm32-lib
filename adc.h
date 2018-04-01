#pragma once

#include "stm32f0xx_conf.h"

#include "gpio.h"
#include "dma.h"


#ifdef STM32F10X_MD
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
#else
#define ADC1_DR_Address    0x40012440
#endif

class Adc;

class AdcChannel
{
public:
	AdcChannel(uint8_t channel, uint8_t numSamples)
		: _channel(channel)
		, next(nullptr)
        , enabled(true)
{}
	uint8_t _channel;
	AdcChannel* next;
	uint32_t _accumulator;
	uint16_t _average;
	bool enabled;
};

class Adc
{
public:
	Adc(ADC_TypeDef* adcx)
		: dmaBuf(nullptr)
		, _peripheral(adcx)
		, _head(nullptr)
	{
		_enableClock();
	};

	void initRegSimul(void);
	void dmaDone(void);
	void waitConversion(void);
	void DmaConfig(void);


//	setNumSamples();

	uint8_t _numChannels;
	static const uint16_t _numSamples = 10;

	volatile uint16_t* dmaBuf;

	void init(	FunctionalState continuousConvMode,
				uint32_t resolution = ADC_Resolution_12b,
				uint32_t extTrigConvEdge = ADC_ExternalTrigConvEdge_None,
				uint32_t extTrigConv = ADC_ExternalTrigConv_T1_TRGO,
				uint32_t dataAlign = ADC_DataAlign_Right,
				uint32_t scanDirection = ADC_ScanDirection_Upward) {
		_config.ADC_ContinuousConvMode = continuousConvMode;
		_config.ADC_Resolution = resolution;
		_config.ADC_ExternalTrigConvEdge = extTrigConvEdge;
		_config.ADC_ExternalTrigConv = extTrigConv;
		_config.ADC_DataAlign = dataAlign;
		_config.ADC_ScanDirection = scanDirection;
		init();
	}

	void init(void) {
		ADC_Init(_peripheral, &_config);
	}


	void dmaConfig(FunctionalState enabled) {
		ADC_DMACmd(_peripheral, enabled);
	}

	void enable();
	void calibrate();

	AdcChannel* addChannel(uint32_t channel);
	void startConversion(void);

private:
	void _enableClock(void);
	ADC_TypeDef* _peripheral;
	AdcChannel* _head;
	ADC_InitTypeDef _config;
};


// Configures the gpio, adc channel sequencing, and dma
AdcChannel* Adc::addChannel(uint32_t channel)
{
	GPIO_TypeDef* gpiox;
	uint8_t pinx;
	switch (channel)
	{
	case ADC_Channel_0...ADC_Channel_7:
	gpiox = GPIOA;
	pinx = 0 + (channel - ADC_Channel_0);
	break;
	default:
		return nullptr;
	}

	// TODO we should insert here instead of counting on them being added in order

	Gpio gpio(gpiox, pinx);
	gpio.init(GPIO_Mode_AN);

	AdcChannel* chanx = new AdcChannel(channel, _numSamples);
	if (!_head) {
		_head = chanx;
	} else {
		AdcChannel* tmp = _head;
		while(tmp->next) tmp = tmp->next;
		tmp->next = chanx;
	}
	_numChannels++;

	  /* ADC1 regular channels configuration */
		AdcChannel* tmp = _head;
		uint8_t rank = 1;
		while (tmp) {
			ADC_ChannelConfig(ADC1, tmp->_channel, ADC_SampleTime_239_5Cycles);
			tmp = tmp->next;
		}
		return chanx;
}



void Adc::_enableClock(void)
{
	ADC_ClockModeConfig(ADC1, ADC_ClockMode_SynClkDiv2);

	switch((uint32_t)_peripheral) {
	case ADC1_BASE:
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
		break;
	default:
		break;
	}
}

void Adc::enable(void)
{
	calibrate();
	ADC_Cmd(ADC1, ENABLE);
	DmaConfig();
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); // wait ready
}

void Adc::waitConversion(void)
{
	  while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
	  DMA_ClearFlag(DMA1_FLAG_TC1);
	  dmaDone();
}

void Adc::startConversion(void)
{
	  ADC_StartOfConversion(ADC1);
}

void Adc::calibrate(void)
{
	ADC_GetCalibrationFactor(ADC1); // blocking on F0
}

void Adc::DmaConfig(void)
{
	DMA_Cmd(DMA1_Channel1, DISABLE);
	if (dmaBuf)
	{
		delete[] dmaBuf;
	}

	dmaBuf = new uint16_t[_numSamples * _numChannels];

	Dma dma1c1 = Dma(DMA1_Channel1);

	dma1c1.init(((uint32_t)ADC1_DR_Address), //ADC1->DR
				(uint32_t)dmaBuf,
				_numChannels * _numSamples,
				DMA_PeripheralDataSize_HalfWord,
				DMA_MemoryDataSize_HalfWord,
				DMA_Mode_Circular,
				DMA_Priority_High,
				DMA_MemoryInc_Enable);

	dma1c1.setEnabled(ENABLE);

	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular); // use oneshot mode for non-continuous conversion
	ADC_DMACmd(ADC1, ENABLE);
}

void Adc::dmaDone(void)
{
	AdcChannel* tmp = _head;

	// buffer size = numChannels * numSamples
	for (uint16_t i = 0; i < _numChannels * _numSamples; i++)
	{
		tmp->_accumulator += dmaBuf[i];
		if (tmp->next) {
			tmp = tmp->next;
		} else {
			tmp = _head;
		}
	}
	tmp = _head;
	while (tmp)
	{
		tmp->_average = tmp->_accumulator / _numSamples;
		tmp->_accumulator = 0;
		tmp = tmp->next;
	}


}

