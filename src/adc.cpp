#include "adc.h"
AdcChannel* Adc::addChannel(uint32_t channel)
{
	GPIO_TypeDef* gpiox;
	uint8_t pinx;
	switch (channel)
	{
#ifdef ADC_Channel_0
	case ADC_Channel_0:
	gpiox = GPIOA;
	pinx = 0;
	break;
#endif
	case ADC_Channel_1...ADC_Channel_7:
	gpiox = GPIOA;
	pinx = 0 + (channel - ADC_Channel_1);
	break;
	case ADC_Channel_10...ADC_Channel_15:
		gpiox = GPIOC;
		pinx = 0 + (channel - ADC_Channel_10);
		break;
	default:
		return nullptr;
	}

	// TODO we should insert here instead of counting on them being added in order

	Gpio gpio(gpiox, pinx);
#if defined(STM32F0) || defined(STM32F3)
	gpio.init(GPIO_Mode_AN, GPIO_PuPd_NOPULL);
#elif defined(STM32F1)
	gpio.init(GPIO_Mode_AIN);
#else
#error
#endif

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
#if defined(STM32F0)
		ADC_ChannelConfig(ADC1, tmp->channel, ADC_SampleTime_239_5Cycles);
#elif defined(STM32F1)
		ADC_RegularChannelConfig(ADC1, tmp->channel, _numChannels, ADC_SampleTime_239Cycles5);
		setSeqNumChannels(_numChannels);
#elif defined(STM32F3)
		ADC_RegularChannelConfig(ADC1, tmp->channel, _numChannels, ADC_SampleTime_601Cycles5);
		setSeqNumChannels(_numChannels);
#else
#endif
		tmp = tmp->next;
	}

	return chanx;
}

#if defined(STM32F0)
void Adc::init(FunctionalState continuousConvMode,
			uint32_t resolution,
			uint32_t extTrigConvEdge,
			uint32_t extTrigConv,
			uint32_t dataAlign,
			uint32_t scanDirection)
	{
		// Configuration
		ADC_InitTypeDef _config;
		ADC_StructInit(&_config);
		_config.ADC_ContinuousConvMode = continuousConvMode;
		_config.ADC_Resolution = resolution;
		_config.ADC_ExternalTrigConvEdge = extTrigConvEdge;
		_config.ADC_ScanDirection = scanDirection;
		_config.ADC_ExternalTrigConv = extTrigConv;
		_config.ADC_DataAlign = dataAlign;
		ADC_Init(_peripheral, &_config);
	}
#elif defined(STM32F1)
void Adc::init(FunctionalState continuousConvMode,
			uint32_t extTrigConv,
			uint32_t dataAlign)
	{
		// Configuration
		ADC_InitTypeDef _config;
		ADC_StructInit(&_config);
		_config.ADC_Mode = ADC_Mode_Independent;
		_config.ADC_ScanConvMode = ENABLE; // not reset value!?
		_config.ADC_ContinuousConvMode = continuousConvMode;
		_config.ADC_ExternalTrigConv = extTrigConv;
		_config.ADC_DataAlign = dataAlign;
		_config.ADC_NbrOfChannel = _numChannels;
		ADC_Init(_peripheral, &_config);
	}
#elif defined(STM32F3)
	void Adc::init(uint32_t continuousConvMode)
	{
		// Configuration
		ADC_InitTypeDef _config;
		ADC_StructInit(&_config);
		_config.ADC_ContinuousConvMode = continuousConvMode;
		_config.ADC_NbrOfRegChannel = _numChannels;
		ADC_Init(_peripheral, &_config);
	}
#else
#error
#endif
void Adc::_enableClock(void)
{
#if defined(STM32F0)
	ADC_ClockModeConfig(_peripheral, ADC_ClockMode_SynClkDiv2);
#elif defined(STM32F1)
	RCC_ADCCLKConfig(RCC_PCLK2_Div4);
#elif defined(STM32F3)
	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div128);

	//ADC_ClockModeConfig(_peripheral, ADC_ClockMode_SynClkDiv2);
#else
#error
#endif
	switch((uint32_t)_peripheral) {
	case ADC1_BASE:
	#if defined(RCC_APB2Periph_ADC1)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	#elif defined(RCC_AHBPeriph_ADC12)
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);
	#else
	#error
	#endif
		break;
	default:
		break;
	}
}

void Adc::enable(void)
{
#if defined(STM32F0) || defined(STM32F3)
	_calibrate();
	//_dmaConfig();
	ADC_Cmd(ADC1, ENABLE);
#elif defined(STM32F1)
	//_dmaConfig();
	ADC_Cmd(ADC1, ENABLE);
	_calibrate();
#else
#error
#endif
	waitReady();
}

void Adc::waitReady(void)
{
#if defined(STM32F0)
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); // wait ready
#elif defined(STM32F1)
	mDelay(10);
#elif defined(STM32F3)
	mDelay(10);

	while (ADC_GetCalibrationStatus(_peripheral));
#else
#error
#endif
}

void Adc::waitConversion(void)
{
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
	DMA_ClearFlag(DMA1_FLAG_TC1);
	update();
}

void Adc::startConversion(void)
{
#if defined(STM32F0)
	ADC_StartOfConversion(_peripheral);
#elif defined(STM32F1)
	ADC_SoftwareStartConvCmd(_peripheral, ENABLE);
#elif defined(STM32F3)
	ADC_StartConversion(_peripheral);
#else
#error
#endif
}

void Adc::_calibrate(void)
{
#if defined(STM32F0)
	ADC_GetCalibrationFactor(ADC1); // blocking on F0
#elif defined(STM32F1) || defined(STM32F3)
//   /* Enable ADC1 reset calibration register */   
//   ADC_ResetCalibration(ADC1);
//   /* Check the end of ADC1 reset calibration register */
//   while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
#else
#error
#endif
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
			DMA_DIR_PeripheralSRC,
			DMA_PeripheralDataSize_HalfWord,
			DMA_MemoryDataSize_HalfWord,
			DMA_Mode_Circular,
			DMA_Priority_High,
			DMA_MemoryInc_Enable);

	dma1c1.setEnabled(ENABLE);

#if defined(STM32F0)
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular); // use oneshot mode for non-continuous conversion
#elif defined(STM32F3)
	ADC_DMAConfig(_peripheral, ADC_DMAMode_Circular);
#else
//#error
#endif
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
