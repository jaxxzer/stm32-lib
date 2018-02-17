#pragma once

#if defined(STM32F0)
#include "stm32f0xx_conf.h"
#elif defined(STM32F1)
#include "stm32f10x_conf.h"
#elif defined(STM32F3)
#include "stm32f30x_conf.h"
#else
 #error
#endif

//#include "stm32lib-conf.h"
#include <inttypes.h>

class Gpio
{
public:
	Gpio(GPIO_TypeDef* port, uint16_t pin);
	void gpioInit(GPIO_InitTypeDef* gpioInitStruct);



#if defined(STM32F0) || defined(STM32F3)
	// Initialization and configuration
	void configAF(uint8_t af);
	void init(GPIOMode_TypeDef mode
			, GPIOPuPd_TypeDef pupd = GPIO_PuPd_NOPULL
			, GPIOOType_TypeDef otype = GPIO_OType_PP
			, GPIOSpeed_TypeDef speed = GPIO_Speed_Level_2);
#elif defined(STM32F1)
	static void remapConfig(uint32_t remap, FunctionalState newstate);
	void init(GPIOMode_TypeDef mode
			, GPIOSpeed_TypeDef speed = GPIO_Speed_50MHz);
#else
#error
#endif
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
