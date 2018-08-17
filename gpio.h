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
