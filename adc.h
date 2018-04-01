#pragma once

  #include "stm32f0xx_conf.h"

#include <stdio.h>

// What do we need for an ADC?
/*
 * - A Gpio
 * - An ADC
 * - An ADC channel
 */
#include "gpio.h"
#if defined (STM32F051x8) || defined (STM32F030)
#include "stm32f0xx_adc.h"
#endif

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
		, _adc(adcx)
		, _head(nullptr)
	{
		_enableClock();
	};

	void init(void);
	void initIndependent(void);
	void initRegSimul(void);
	AdcChannel* addChannel(uint8_t channel);
	void startConversion(void);
	void dmaDone(void);
	void waitConversion(void);
	void DmaConfig(void);
	void enable();
	void calibrate();

	static uint8_t _numChannels;
	static uint16_t _numSamples;

	volatile uint16_t* dmaBuf;


private:
	void _enableClock(void);
	ADC_TypeDef* _adc;
	AdcChannel* _head;

};

void Adc::init(void)
{
	calibrate();
	enable();
	DmaConfig();
	startConversion();
	waitConversion();
}

AdcChannel* Adc::addChannel(uint8_t channel)
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

	DMA_Cmd(DMA1_Channel1, DISABLE);
	if (dmaBuf)
	{
		delete[] dmaBuf;
	}

	dmaBuf = new uint16_t[_numSamples * _numChannels];

	initRegSimul();
	return chanx;
}

//const uint8_t MAX_CHANNELS = 8;
//const uint8_t MAX_SAMPLES = 40;
uint8_t Adc::_numChannels = 0;
uint16_t Adc::_numSamples = 100;
//__IO uint32_t ADC_DualConvertedValueTab[MAX_CHANNELS * MAX_SAMPLES];

void Adc::_enableClock(void)
{
#if defined (STM32F051x8) || defined (STM32F030)
	ADC_ClockModeConfig(ADC1, ADC_ClockMode_AsynClk);
#else
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
#endif
	switch((uint32_t)_adc) {
	case ADC1_BASE:
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
		break;
#ifdef STM32F10X_MD
	case ADC2_BASE:
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
		break;
#endif
	default:
		break;
	}
}

void Adc::initRegSimul(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	ADC_StructInit(&ADC_InitStructure);

	  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
//	  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	  ADC_Init(ADC1, &ADC_InitStructure);

	  /* ADC1 regular channels configuration */
		AdcChannel* tmp = _head;
		uint8_t rank = 1;
		while (tmp) {
#ifdef STM32F10X_MD
			ADC_RegularChannelConfig(ADC1, tmp->_channel, rank++, ADC_SampleTime_239Cycles5);
#else
			// F0 doesnt let you program the "rank"
			ADC_ChannelConfig(ADC1, tmp->_channel, ADC_SampleTime_239_5Cycles);
#endif
			tmp = tmp->next;
		}

}
void Adc::enable(void)
{
	ADC_Cmd(ADC1, ENABLE);
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
	if (!ADC_GetCalibrationFactor(ADC1)) {
	  printf("ADC failed to calibrate");
	}
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
	ADC_ClockModeConfig(ADC1, ADC_ClockMode_SynClkDiv2);
}

void Adc::DmaConfig(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel1);

	DMA_InitTypeDef DMA_InitStructure;
	DMA_StructInit(&DMA_InitStructure);
	/* DMA1 channel1 configuration ----------------------------------------------*/
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ((uint32_t)ADC1_DR_Address);//ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)dmaBuf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = _numChannels * _numSamples;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // TODO halfword
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	/* Enable DMA1 Channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
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

