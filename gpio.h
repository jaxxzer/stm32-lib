#pragma once

#include "stm32f0xx_conf.h"

class Gpio
{
public:
	Gpio(GPIO_TypeDef* port, uint16_t pin);
	void gpioInit(GPIO_InitTypeDef* gpioInitStruct);

	// Initialization and configuration
	void configAF(uint8_t af);

	void init(GPIOMode_TypeDef mode
			, GPIOPuPd_TypeDef pupd = GPIO_PuPd_NOPULL
			, GPIOOType_TypeDef otype = GPIO_OType_PP
			, GPIOSpeed_TypeDef speed = GPIO_Speed_Level_2);

	void init(void);

	// Read
	bool readOutput(void);
	bool readInput(void);

	// Write
	void set(bool set = true);
	void reset(void);
	void toggle(void);

	void setInversion(bool inverted) { _inverted = inverted; }

private:
	void _clockEnable(void);

	GPIO_TypeDef* _port;
	uint16_t _pin;
	uint16_t _pinSource;

	bool _inverted;

	//GPIO_InitTypeDef _config;
};

void Gpio::init(GPIOMode_TypeDef mode // GPIO_Mode_IN, GPIO_Mode_OUT, GPIO_Mode_AF, GPIO_Mode_AN
			, GPIOPuPd_TypeDef pupd
			, GPIOOType_TypeDef otype
			, GPIOSpeed_TypeDef speed)
{
	GPIO_InitTypeDef _config;
	_config.GPIO_Mode = mode;
	_config.GPIO_PuPd = pupd;
	_config.GPIO_OType = otype;
	_config.GPIO_Speed = speed;
	_config.GPIO_Pin = (_pin);
	GPIO_Init(_port, &_config);
	reset();
}

Gpio::Gpio(GPIO_TypeDef* port, uint16_t pin)
	: _port(port)
	, _pin(1<<pin)
    , _pinSource(pin)
{
	_clockEnable();
	//GPIO_StructInit(&_config);
}

void Gpio::_clockEnable(void)
{
	switch ((uint32_t)_port) {
#if defined (STM32F051x8) || defined (STM32F030)
	case GPIOA_BASE:
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	    break;
	case GPIOB_BASE:
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
		break;
	case GPIOC_BASE:
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
		break;
	case GPIOD_BASE:
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
		break;
#else
	case GPIOA_BASE:
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	    break;
	case GPIOB_BASE:
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		break;
	case GPIOC_BASE:
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		break;
	case GPIOD_BASE:
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
		break;
#endif

	default:
		break; // failure
	}
}

//void Gpio::init(void)
//{
//	_config.GPIO_Pin = (_pin);
//	GPIO_Init(_port, &_config);
//}

//void Gpio::gpioInit(GPIO_InitTypeDef* gpioInitStruct) {
//	_config = *gpioInitStruct;
//	init(); // TODO warn if pin mismatch
//}

// alternate function config
// See datasheet section 4 'Pinouts and pin descriptions'
void Gpio::configAF(uint8_t af) {
    GPIO_PinAFConfig(_port, _pinSource, af);
}

void Gpio::set(bool set)
{
	bool state = _inverted ? !set : set;

	if (state) {
		//GPIO_SetBits(_port, _pin);
		_port->BSRR = _pin;
	} else {
		_port->BRR = _pin;
	}
}

void Gpio::reset(void)
{
	//GPIO_ResetBits(_port, _pin);
	set(false);
}

bool Gpio::readOutput(void)
{
	//return GPIO_ReadOutputDataBit(_port, _pin);
	bool state = _port->ODR & _pin;
	return _inverted ? !state : state;
}

bool Gpio::readInput(void)
{
	//return GPIO_ReadInputDataBit(_port, _pin);
	bool state = _port->IDR & _pin;
	return _inverted ? !state : state;
}


void Gpio::toggle(void)
{
	BitAction new_state = (BitAction)!readOutput();
	//GPIO_WriteBit(_port, _pin, new_state);

	if (new_state != Bit_RESET) {
		set(true);
	} else {
		set(false);
	}
}
