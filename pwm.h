/*
 * pwm.h
 *
 *  Created on: Feb 7, 2018
 *      Author: jack
 */



#ifndef PWM_H_
#define PWM_H_
#include "gpio.h"
#include "timer.h"
// What do we need for a pwm?
/*
 * - a GPIO
 * - a Timer
 * - a #free# timer channel
 * 

 * 
 * 
 * What describes the state of a pwm (in addition to inherited parameters)?
 * - active (currently pwming)
 * - pwm width (s)
 * - pwm frequency (Hz)
 */
class Pwm
{
public:
	Pwm(Gpio* gpio, Timer* timer, uint8_t channel)
        : _gpio(gpio)
        , _timer(timer)
        , _channel(channel)
	{};
	void init(void);
	void initComplimentary(void);
    void setDutyCycle(uint16_t pulse);
    void enable(void);
    void disable(void);
    void toggle(void);

private:
	Gpio* _gpio;
	Timer* _timer;
    uint8_t _channel;
    uint16_t _dutyCycle;

    void _gpioInit(void);
};

// Timer output channel initialization
// * - initialize GPIO as AF output
// * - initialize timer
// - Configure timer output channel
// - Enable timer output channel
//
// Requires: Timer peripheral and output channel

void Pwm::_gpioInit(void) {

	_gpio->initAFPP();
}

void Pwm::init(void)
{
	_gpioInit();

	_timer->init(50000, 4800000); // period, frequency
	_timer->outputChannelInitPwm(_channel, _dutyCycle);
}

void Pwm::initComplimentary(void)
{
	_gpioInit();
	_timer->init(50000, 48000000); // period, frequency

	_timer->outputChannelInitPwmComplimentary(_channel, _dutyCycle);

}



// Duty cycle in ticks
void Pwm::setDutyCycle(uint16_t pulse) {
    _dutyCycle = pulse;
    _timer->setDutyCycle(_channel, _dutyCycle);
}

void Pwm::enable(void)
{

}

void Pwm::disable(void)
{

}

void Pwm::toggle(void)
{

}


#endif /* PWM_H_ */
