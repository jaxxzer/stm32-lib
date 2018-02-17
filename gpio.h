/*
 * gpio.h
 *
 *  Created on: Feb 7, 2018
 *      Author: jack
 */

#ifndef GPIO_H_
#define GPIO_H_

//#include <string.h>


// What do we need for a GPIO?
/*
 * - A port
 * - A pin
 *
 * What do we need to init a GPIO?
 * - A GPIO_InitTypeDef
 *
 * What describes the state of the GPIO?
 *
 * - High/Low state
 */



// To initialize a GPIO:
/*
 * Enable the peripheral clock
 * Initialize the GPIO
 */




class Gpio
{
public:
	Gpio(GPIO_TypeDef* port, uint16_t pin);
	void gpioInit(GPIO_InitTypeDef* gpioInitStruct);
	void initAFPP(void);
	void initInFloating(void);
	void initOutputPP(void);
	void initSwoRemapped(void);
	void initAnalogIn(void);
	void init(void);
	bool deinit(void);
	void set(void);
	void reset(void);
	void toggle(void);
	bool state(void);
private:
	void _clockEnable(void);
	GPIO_TypeDef* _port;
	uint16_t _pin;
	GPIO_InitTypeDef _configuration;

};

Gpio::Gpio(GPIO_TypeDef* port, uint16_t pin)
	: _port(port)
	, _pin(pin)
{
	_clockEnable();
}

void Gpio::_clockEnable(void)
{
	switch ((uint32_t)_port) {
#ifdef FUCKME
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
#endif
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
	default:
		break; // failure
	}
}

void Gpio::init(void)
{
	_configuration.GPIO_Pin = _pin;
	_configuration.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_port, &_configuration);
}

void Gpio::gpioInit(GPIO_InitTypeDef* gpioInitStruct) {
	_configuration = *gpioInitStruct;
	init();
}

void Gpio::initAFPP(void)
{
#ifdef FUCKME
	_configuration.GPIO_Mode = GPIO_Mode_OUT;
	_configuration.GPIO_OType = GPIO_OType_PP;
	_configuration.GPIO_Speed = GPIO_Speed_2MHz;
#endif
	_configuration.GPIO_Mode = GPIO_Mode_AF_PP;
	init();
}

void Gpio::initInFloating(void)
{
	_configuration.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	_configuration.GPIO_Mode = GPIO_Mode_AF_OD;
	init();
}
void Gpio::initOutputPP(void)
{
#ifdef FUCKME
	_configuration.GPIO_Mode = GPIO_Mode_OUT;
	_configuration.GPIO_OType = GPIO_OType_PP;
	_configuration.GPIO_Speed = GPIO_Speed_2MHz;
#endif
	_configuration.GPIO_Mode = GPIO_Mode_Out_PP;

	init();
}

void Gpio::initSwoRemapped(void)
{

	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);
	initAFPP();
}

void Gpio::initAnalogIn(void)
{
	_configuration.GPIO_Mode = GPIO_Mode_AIN;

	init();
}


void Gpio::set(void)
{
	GPIO_SetBits(_port, _pin);
}

void Gpio::reset(void)
{
	GPIO_ResetBits(_port, _pin);
}

bool Gpio::state(void)
{
	return GPIO_ReadOutputDataBit(_port, _pin);
}

void Gpio::toggle(void)
{
	BitAction new_state = (BitAction)!state();
	GPIO_WriteBit(_port, _pin, new_state);
}

#endif /* GPIO_H_ */
