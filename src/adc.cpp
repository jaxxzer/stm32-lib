#include "adc.h"
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
