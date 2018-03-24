/*
 * gpio.h
 *
 *  Created on: Feb 7, 2018
 *      Author: jack
 */

#pragma once

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


  #include "stm32f0xx_conf.h"



class Gpio
{
public:
	Gpio(GPIO_TypeDef* port, uint16_t pin);
	void gpioInit(GPIO_InitTypeDef* gpioInitStruct);
	void initAFPP(void);
	void initInFloating(void);
	void initInPullUp(void);
	void initInPullDown(void);
	void initOutputPP(void);
	void initSwoRemapped(void);
	void initAnalogIn(void);
	void _init(void);
	bool deinit(void);
	void set(bool set = true);
	void reset(void);
	void toggle(void);
	bool readOutput(void);
	bool readInput(void);
	void configAF(uint8_t af);

	void init(GPIOMode_TypeDef mode = GPIO_Mode_IN
			, GPIOPuPd_TypeDef pupd = GPIO_PuPd_NOPULL
			, GPIOOType_TypeDef otype = GPIO_OType_PP
			, GPIOSpeed_TypeDef speed = GPIO_Speed_Level_2);

private:
	void _clockEnable(void);
	GPIO_TypeDef* _port; //TODO const
	uint16_t _pin;//TODO const
	uint16_t _pinSource;//TODO const

//

//	typedef struct
//	{
//	  uint32_t GPIO_Pin;              /*!< Specifies the GPIO pins to be configured.
//	                                       This parameter can be any value of @ref GPIO_pins_define */
//
//	  GPIOMode_TypeDef GPIO_Mode;     /*!< Specifies the operating mode for the selected pins.
//	                                       This parameter can be a value of @ref GPIOMode_TypeDef   */
//
//	  GPIOSpeed_TypeDef GPIO_Speed;   /*!< Specifies the speed for the selected pins.
//	                                       This parameter can be a value of @ref GPIOSpeed_TypeDef  */
//
//	  GPIOOType_TypeDef GPIO_OType;   /*!< Specifies the operating output type for the selected pins.
//	                                       This parameter can be a value of @ref GPIOOType_TypeDef  */
//
//	  GPIOPuPd_TypeDef GPIO_PuPd;     /*!< Specifies the operating Pull-up/Pull down for the selected pins.
//	                                       This parameter can be a value of @ref GPIOPuPd_TypeDef   */
//	}GPIO_InitTypeDef;
	GPIO_InitTypeDef _config;

};

void Gpio::init(GPIOMode_TypeDef mode
			, GPIOPuPd_TypeDef pupd
			, GPIOOType_TypeDef otype
			, GPIOSpeed_TypeDef speed)
{
	_config.GPIO_Mode = mode;
	_config.GPIO_PuPd = pupd;
	_config.GPIO_OType = otype;
	_config.GPIO_Speed = speed;
	_init();
}

Gpio::Gpio(GPIO_TypeDef* port, uint16_t pin)
	: _port(port)
	, _pin(1<<pin)
    , _pinSource(pin)
{
	_clockEnable();
	GPIO_StructInit(&_config);
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

void Gpio::_init(void)
{
	_config.GPIO_Pin = (_pin);
	GPIO_Init(_port, &_config);
#if defined (STM32F051x8) || defined (STM32F030)

#endif
}

void Gpio::gpioInit(GPIO_InitTypeDef* gpioInitStruct) {
	_config = *gpioInitStruct;
	_init(); // TODO warn if pin mismatch
}

void Gpio::configAF(uint8_t af) {
    GPIO_PinAFConfig(_port, _pinSource, af);
}

void Gpio::initInPullUp(void)
{
#if defined (STM32F051x8) || defined (STM32F030)
	_config.GPIO_Mode = GPIO_Mode_IN;
	_config.GPIO_PuPd = GPIO_PuPd_UP;
	#else
	_config.GPIO_Mode = GPIO_Mode_IPU;
#endif
	_init();
}

void Gpio::initInPullDown(void)
{
#if defined (STM32F051x8) || defined (STM32F030)
	_config.GPIO_Mode = GPIO_Mode_IN;
	_config.GPIO_PuPd = GPIO_PuPd_DOWN;
	#else
	_config.GPIO_Mode = GPIO_Mode_IPD;
#endif
	_init();
}

void Gpio::initAFPP(void)
{
#if defined (STM32F051x8) || defined (STM32F030)
	_config.GPIO_Mode = GPIO_Mode_AF;
	_config.GPIO_OType = GPIO_OType_PP;
#else
	_config.GPIO_Mode = GPIO_Mode_AF_PP;
#endif
	_init();
}

void Gpio::initInFloating(void)
{
#if defined (STM32F051x8) || defined (STM32F030)
	_config.GPIO_Mode = GPIO_Mode_IN;
#else
	_config.GPIO_Mode = GPIO_Mode_IN_FLOATING;
#endif
	_init();
}
void Gpio::initOutputPP(void)
{
#if defined (STM32F051x8) || defined (STM32F030)
	_config.GPIO_Mode = GPIO_Mode_OUT;
	_config.GPIO_OType = GPIO_OType_PP;
#else
	_configuration.GPIO_Mode = GPIO_Mode_Out_PP;
#endif
	_init();
}

void Gpio::initSwoRemapped(void)
{

#ifdef STM32F10X_MD
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);
#endif
	initAFPP();
}

void Gpio::initAnalogIn(void)
{
#ifdef STM32F10X_MD
	_config.GPIO_Mode = GPIO_Mode_AIN;
#else
	_config.GPIO_Mode = GPIO_Mode_AN;
#endif

	_init();
}


void Gpio::set(bool set)
{
	if (set) {
		GPIO_SetBits(_port, _pin);
	} else {
		reset();
	}
}

//void Gpio::enableInterrupt(void)
//{
//
//}
void Gpio::reset(void)
{
	GPIO_ResetBits(_port, _pin);
}

bool Gpio::readOutput(void)
{
	return GPIO_ReadOutputDataBit(_port, _pin);
}

bool Gpio::readInput(void)
{
	return GPIO_ReadInputDataBit(_port, _pin);
}


void Gpio::toggle(void)
{
	BitAction new_state = (BitAction)!readOutput();
	GPIO_WriteBit(_port, _pin, new_state);
}
