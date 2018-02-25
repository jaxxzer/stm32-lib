// What do we need for an ADC?
/*
 * - A Gpio
 * - An ADC
 * - An ADC channel
 */
#include "gpio.h"
#ifdef STM32F051x8
#include "stm32f0xx_adc.h"
#endif

#ifdef STM32F10X_MD
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
#else
#define ADC1_DR_Address    0x40012440
#endif

class AdcChannel
{
public:
	AdcChannel(Gpio* gpiox, uint8_t channel, uint8_t numSamples)
		: _gpio(gpiox)
		, _channel(channel)
		, next(NULL)
{}
	Gpio* _gpio;
	uint8_t _channel;
	AdcChannel* next;
	uint32_t _accumulator;
	uint16_t _average;
};

class Adc
{
public:
	Adc(ADC_TypeDef* adcx)
		: dmaBuf(NULL)
		, _adc(adcx)
		, _head(NULL)
	{
		_enableClock();
	};

	void initIndependent(void);
	void initRegSimul(void);
	AdcChannel* addChannel(uint8_t channel);
	void startConversion(void);
	void dmaDone(void);
	void waitConversion(void);
	void DmaConfig(void);
	void enable();
	void waitReady();
	void calibrate();


	static uint8_t _numChannels;
	static uint8_t _numSamples;

	volatile uint16_t* dmaBuf;


private:
	void _enableClock(void);
	ADC_TypeDef* _adc;
	AdcChannel* _head;

};

AdcChannel* Adc::addChannel(uint8_t channel)
{
	GPIO_TypeDef* gpiox;
	uint8_t pinx;
	switch (channel)
	{
	case ADC_Channel_0...ADC_Channel_7:
		gpiox = GPIOA;
		pinx = GPIO_Pin_0 + (channel - ADC_Channel_0);
		break;
	default:
		return NULL;
	}

	Gpio* gpio = new Gpio(gpiox, pinx);
	gpio->initAnalogIn();
	AdcChannel* chanx = new AdcChannel(gpio, channel, _numSamples);
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
uint8_t Adc::_numSamples = 2;
//__IO uint32_t ADC_DualConvertedValueTab[MAX_CHANNELS * MAX_SAMPLES];

void Adc::_enableClock(void)
{
#ifdef STM32F051x8
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
// Enable GPIO peripheral clock
// Initialize GPIO
// Enable ADC interrupt
// Enable ADC peripheral clock
// Initialize ADC peripheral
// Enable ADC peripheral
// Calibrate ADC
void Adc::initIndependent(void) {
//	// enable gpio clock
//	_gpio->initAnalogIn();
//
//	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//
//	_enableClock();
//
//	ADC_InitTypeDef ADC_InitStruct;
//	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
//	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
//	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
//	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
//	ADC_InitStruct.ADC_NbrOfChannel = 1;
//	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
//
//	ADC_Init(_adc, &ADC_InitStruct);
//	/* Enable ADC1 */
//	ADC_Cmd(_adc, ENABLE);
//
//	/* Enable ADC1 reset calibaration register */
//	ADC_ResetCalibration(_adc);
//	/* Check the end of ADC1 reset calibration register */
//	while(ADC_GetResetCalibrationStatus(_adc));
//	/* Start ADC1 calibaration */
//	ADC_StartCalibration(_adc);
//	/* Check the end of ADC1 calibration */
//	while(ADC_GetCalibrationStatus(_adc));
//	ADC_ITConfig(_adc, ADC_IT_EOC, ENABLE);
//
//	ADC_RegularChannelConfig(_adc, _channel, 1, ADC_SampleTime_71Cycles5); // storm
}



void Adc::initRegSimul(void)
{
	  //ADC_Cmd(ADC1, DISABLE);



	ADC_InitTypeDef ADC_InitStructure;
	ADC_StructInit(&ADC_InitStructure);


#ifdef STM32F10X_MD
	  /* ADC1 configuration ------------------------------------------------------*/
	  ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
	  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  ADC_InitStructure.ADC_NbrOfChannel = _numChannels;

#else
	  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
//	  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
#endif
	  ADC_Init(ADC1, &ADC_InitStructure);

	  /* ADC1 regular channels configuration */
		AdcChannel* tmp = _head;
		uint8_t rank = 1;
		while (tmp) {
#ifdef STM32F10X_MD
			ADC_RegularChannelConfig(ADC1, tmp->_channel, rank++, ADC_SampleTime_239Cycles5);
#else
			// F0 doesnt let you program the "rank"
			ADC_ChannelConfig(ADC1, tmp->_channel, ADC_SampleTime_1_5Cycles);
#endif
			tmp = tmp->next;
		}

}
void Adc::enable(void)
{
	ADC_Cmd(ADC1, ENABLE);
	waitReady();
}

void Adc::waitReady(void)
{
#ifdef STM32F10X_MD
	DelayMil(1); //tStab??
#else
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY));
#endif
;
}

void Adc::waitConversion(void)
{
	  /* Test on DMA1 channel1 transfer complete flag */
	  while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
	  DMA_ClearFlag(DMA1_FLAG_TC1);
	  // This is how you poll continuously in DMA_Mode_Normal
	  //DMA_Cmd(DMA1_Channel1, DISABLE);

	  dmaDone();


//	  while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
//	  /* Clear DMA1 channel1 transfer complete flag */
}

void Adc::startConversion(void)
{
#ifdef STM32F10X_MD
	  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
#else
	  ADC_StartOfConversion(ADC1);
#endif
}

void Adc::calibrate(void)
{
#ifdef STM32F10X_MD


	  /* Check the end of ADC1 reset calibration register */
	  while(ADC_GetResetCalibrationStatus(ADC1));

	  /* Start ADC1 calibration */
	  ADC_StartCalibration(ADC1);
	  /* Check the end of ADC1 calibration */
	  while(ADC_GetCalibrationStatus(ADC1));
#else

	if (!ADC_GetCalibrationFactor(ADC1)) {
	  printf("ADC failed to init channel %d", tmp->_channel);
	};
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
	ADC_ClockModeConfig(ADC1, ADC_ClockMode_SynClkDiv2);
#endif
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

