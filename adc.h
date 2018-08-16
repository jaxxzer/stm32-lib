#pragma once

#include "stm32f0xx_conf.h"

#include "gpio.h"
#include "dma.h"

class Adc;

class AdcChannel
{
	friend class Adc;

public:
	AdcChannel(uint8_t channel, uint8_t numSamples)
		: channel(channel)
		, next(nullptr)
	{}

	uint8_t channel;
	uint32_t accumulator;
	uint16_t average;

private:
	AdcChannel* next;
};

// This class is used to initialize and configure an ADC peripheral
// ADC channels are handling internally as a linked list. This facilitates
// configuration and handling of regular and injected channel sequencing
//
// There is only one ADC output data register shared by the individual ADC
// channels. In order to handle more than one ADC channel, a buffer must
// be used in conjuction with DMA transfers. This class always uses DMA
// even when only one channel is being scanned.
//
// Because this use case is so common, this class takes several liberties
// to set it up, including:
// - Finding and configuring the appropriate GPIO.
// - Allocating memory for DMA transfers
// - Configuring DMA1Channel1 for the transfers.
//
// To use this class:
// - Call addChannel to configure the desired ADC channels
// - Call init
// - Call enable
// - Call startConversion
// - Call waitConversion to wait for the current conversion to complete
// 	 and channel averages to be updated (blocking)
// - Or initialize in continuous mode and call update (non blocking)
class Adc
{
public:
	Adc(ADC_TypeDef* adcx)
		: _peripheral(adcx)
		, _head(nullptr)
		, _dmaBuf(nullptr)
	{
		_enableClock();
	}

	//  15012	    156	    412	  15580	   3cdc	BlueBMS.elf

	// Initialize peripheral with common/default config
	// More commonly used arguments are listed first
	// Default arguments are the same as performed by ADC_StructInit()
	void init(	FunctionalState continuousConvMode,
			uint32_t resolution = ADC_Resolution_12b,
			uint32_t extTrigConvEdge = ADC_ExternalTrigConvEdge_None,
			uint32_t extTrigConv = ADC_ExternalTrigConv_T1_TRGO,
			uint32_t dataAlign = ADC_DataAlign_Right,
			uint32_t scanDirection = ADC_ScanDirection_Upward)
	{
		// Configuration
		ADC_InitTypeDef _config;
		_config.ADC_ContinuousConvMode = continuousConvMode;
		_config.ADC_Resolution = resolution;
		_config.ADC_ExternalTrigConvEdge = extTrigConvEdge;
		_config.ADC_ExternalTrigConv = extTrigConv;
		_config.ADC_DataAlign = dataAlign;
		_config.ADC_ScanDirection = scanDirection;
		ADC_Init(_peripheral, &_config);
	}

	// Enable the ADC
	void enable();

	// Add another channel to the conversion sequence
	// This configures GPIO for you
	// Returns the resulting AdcChannel for access to conversion results
	AdcChannel* addChannel(uint32_t channel);

	// Start a conversion
	void startConversion(void);

	// Wait for the current ADC conversion sequence to complete
	void waitConversion(void);

	// Update average of all channels
	void update(void);

	// TODO Implement
	// Number of samples to be averaged for each channel update
	void setNumSamples(uint16_t samples)
	{
		//_numSamples = samples;
	}

private:
	ADC_TypeDef* _peripheral; // Eg. ADC1, ADC2...
	AdcChannel* _head; // First channel in sequence

	// Number of channels we are currently sequencing
	uint8_t _numChannels;

	// Number of samples to be averaged for each channel update
	uint16_t _numSamples = 50;

	// Buffer to dump conversion results
	volatile uint16_t* _dmaBuf;

	// Enable the ADC peripheral clock
	void _enableClock(void);

	// Calibrate the ADC peripheral
	// This is blocking
	// This should be called before the peripheral is enabled
	void _calibrate();

	// Set up the dma buffer, adc requests, and dma channel
	void _dmaConfig(void);
};

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

	// Insert the channel into the list
	if (!_head) {
		_head = chanx;
	} else {
		AdcChannel* tmp = _head;
		while(tmp->next) tmp = tmp->next;
		tmp->next = chanx;
	}

	_numChannels++;

	AdcChannel* tmp = _head;

	// TODO configure only the added channel
	// Configure all channels
	while (tmp) {
		ADC_ChannelConfig(ADC1, tmp->channel, ADC_SampleTime_239_5Cycles);
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
	_calibrate();
	ADC_Cmd(ADC1, ENABLE);
	_dmaConfig();
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); // wait ready
}

void Adc::waitConversion(void)
{
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
	DMA_ClearFlag(DMA1_FLAG_TC1);
	update();
}

void Adc::startConversion(void)
{
	ADC_StartOfConversion(ADC1);
}

void Adc::_calibrate(void)
{
	ADC_GetCalibrationFactor(ADC1); // blocking on F0
}

void Adc::_dmaConfig(void)
{
	DMA_Cmd(DMA1_Channel1, DISABLE);
	if (_dmaBuf)
	{
		delete[] _dmaBuf;
	}

	_dmaBuf = new uint16_t[_numSamples * _numChannels];

	Dma dma1c1 = Dma(DMA1_Channel1);

	dma1c1.init((uint32_t)&(ADC1->DR),
			(uint32_t)_dmaBuf,
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

void Adc::update(void)
{
	AdcChannel* tmp = _head;

	// Iterate across entire buffer
	// Sum samples for each individual channel
	for (uint16_t i = 0; i < _numChannels * _numSamples; i++) {
		tmp->accumulator += _dmaBuf[i];
		if (tmp->next) {
			tmp = tmp->next;
		} else {
			tmp = _head;
		}
	}

	tmp = _head;

	// Iterate across channels in sequence
	// Calculate average sample value for each individual channel
	while (tmp) {
		tmp->average = tmp->accumulator / _numSamples;
		tmp->accumulator = 0;
		tmp = tmp->next;
	}
}
