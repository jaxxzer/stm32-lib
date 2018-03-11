/*
 * timer.h
 *
 *  Created on: Feb 7, 2018
 *      Author: jack
 */

// What do we need for a timer?
/*
* - A timer peripheral
*/
#pragma once

class TimerChannel
{
public:
	TimerChannel(TIM_TypeDef* timx, uint8_t channel)
		: _peripheral(timx)
        ,_channel(channel)
	{
		TIM_OCStructInit(&_config);
	};

	void enableInterrupt(void);
	void disableInterrupt(void);

	void setDeadtime(uint16_t deadTime);
	void setDutyCycle(float duty);
	void setCompare(uint16_t ccr);

	void preloadConfig(FunctionalState enabled);
	void setEnabled(bool enabled);
	void setEnabledN(bool enabled);

	void setPolarity(bool high);
	void setPolarityN(bool high);

	void setIdleState(bool high);
	void setIdleStateN(bool high);

	void _init(void);

	TIM_TypeDef* _peripheral;
	uint8_t _channel;

	TIM_OCInitTypeDef _config;
};

void TimerChannel::setDutyCycle(float duty)
{
	uint16_t compare = map(duty, 0, 1.0f, 0, _peripheral->ARR);
	setCompare(compare);
}

void TimerChannel::setCompare(uint16_t compare)
{
	_config.TIM_Pulse = compare;
    switch (_channel) {
    case TIM_Channel_1:
        TIM_SetCompare1(_peripheral, compare);
    	break;
    case TIM_Channel_2:
        TIM_SetCompare2(_peripheral, compare);
    	break;
    case TIM_Channel_3:
        TIM_SetCompare3(_peripheral, compare);
    	break;
    case TIM_Channel_4:
        TIM_SetCompare4(_peripheral, compare);
    	break;
    default:
    	break;
    }
}
void TimerChannel::setEnabled(bool enabled)
{
	_config.TIM_OutputState = enabled ? TIM_OutputState_Enable : TIM_OutputState_Disable;
	_init();
}

void TimerChannel::setEnabledN(bool enabled)
{
    _config.TIM_OutputNState = enabled ? TIM_OutputNState_Disable : TIM_OutputNState_Disable;
    _init();
}

void TimerChannel::preloadConfig(FunctionalState enabled)
{
	uint16_t config = enabled ? TIM_OCPreload_Enable : TIM_OCPreload_Disable;

    switch (_channel) {
    case TIM_Channel_1:
        TIM_OC1PreloadConfig(_peripheral, config);
    	break;
    case TIM_Channel_2:
        TIM_OC2PreloadConfig(_peripheral, config);
    	break;
    case TIM_Channel_3:
        TIM_OC3PreloadConfig(_peripheral, config);
    	break;
    case TIM_Channel_4:
        TIM_OC4PreloadConfig(_peripheral, config);
    	break;
    default:
    	break;
    }
}

void TimerChannel::_init(void)
{
    switch (_channel) {
    case TIM_Channel_1:
        TIM_OC1Init(_peripheral, &_config);
    	break;
    case TIM_Channel_2:
        TIM_OC2Init(_peripheral, &_config);
    	break;
    case TIM_Channel_3:
        TIM_OC3Init(_peripheral, &_config);
    	break;
    case TIM_Channel_4:
        TIM_OC4Init(_peripheral, &_config);
    	break;
    default:
    	break;
    }
}

class Timer
{
public:
    Timer(TIM_TypeDef* timx)
        : _peripheral(timx)
    {
    	TIM_TimeBaseStructInit(&_config);
    };

    void init(void);

    void setClockEnabled(FunctionalState enabled);
    void setEnabled(FunctionalState enabled);
    void preloadConfig(FunctionalState enabled);
    void setARR();
    void setFrequency(uint16_t Hz);
	void setPeriod(uint32_t microseconds);
    uint16_t getPeriod(void);

	void setPrescaler(uint16_t prescaler);

    void enableInterrupt(uint16_t interrupt, uint8_t priority);
    void playNote(uint16_t frequency, uint16_t duration_ms);

    TIM_TypeDef* peripheral(void) { return _peripheral; };

private:
    TIM_TypeDef* _peripheral;
    TIM_TimeBaseInitTypeDef _config;
};


// Input: period (ticks), tick frequency (Hz)
// - Enable the peripheral clock
// - Configure the peripheral
// Fails if: SystemCoreClock is not divisible by input frequency


uint16_t Timer::getPeriod(void)
{
	return _config.TIM_Period;
}

void Timer::setClockEnabled(FunctionalState enabled)
{
    switch((uint32_t)_peripheral)
    {
    case TIM1_BASE:
    	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, enabled);
        break;
    case TIM2_BASE:
    	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, enabled);
        break;
    case TIM3_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, enabled);
        break;
    case TIM6_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, enabled);
        break;
    case TIM14_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, enabled);
        break;
//    case TIM15_BASE:
//        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM15, ENABLE);
//        break;
//    case TIM16_BASE:
//        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM16, ENABLE);
//        break;
//    case TIM17_BASE:
//        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM17, ENABLE);
//        break;
    default:
        break;
    }
}

void Timer::setEnabled(FunctionalState enabled)
{
	TIM_Cmd(_peripheral, enabled);
}

void Timer::init(void)
{
    TIM_TimeBaseInit(_peripheral, &_config);
}

void Timer::playNote(uint16_t frequency, uint16_t duration_ms)
{
	setFrequency(frequency);
	DelayMil(duration_ms);
}

void Timer::setPrescaler(uint16_t prescaler)
{
	_config.TIM_Prescaler = prescaler;
	TIM_PrescalerConfig(_peripheral, prescaler, TIM_PSCReloadMode_Immediate);
}

void Timer::setFrequency(uint16_t f) // Hz
{
//	float period = 1.0f/f;
//	float tickPeriod = 1.0f/_tickFrequency;
//	uint16_t ticks = period/tickPeriod;
	RCC_ClocksTypeDef RCC_ClocksStruct;
	RCC_GetClocksFreq(&RCC_ClocksStruct);
	print_clocks();
//	uint16_t ticks = HSI / f;

//	setPeriod(ticks);
}



// period in ticks
void Timer::setPeriod(uint32_t microseconds)
{
//	TIM_SetAutoreload(_peripheral, _period);
}

// Timer peripheral
// Timer interrupt sources
void Timer::enableInterrupt(uint16_t interrupt, uint8_t priority)
{
//	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = priority;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//
//	switch (interrupt) {
//	case TIM_IT_Update:
//    	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
//		break;
//    case TIM_IT_CC1:
//    	break;
//    case TIM_IT_CC2:
//    	break;
//    case TIM_IT_CC3:
//    	break;
//    case TIM_IT_CC4:
//    	break;
//    case TIM_IT_COM:
//    	break;
//    case TIM_IT_Trigger:
//    	break;
//    case TIM_IT_Break:
//    	break;
//    default:
//
//    	break;
//
//    }
//
//	NVIC_Init(&NVIC_InitStructure);
//
//	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

}

// frequency
void InitializeFrequencyTimer(uint16_t period = 1000)// AKA TIMx_ARR = 500000 microseconds = 2Hz
{
    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = (SystemCoreClock / 10000) - 1; // Every 100 microseconds
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = period;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &timerInitStructure);
    TIM_Cmd(TIM1, ENABLE);
}
