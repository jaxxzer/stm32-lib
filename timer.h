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
	{};
	TIM_TypeDef* _peripheral;
	uint8_t _channel;

};

class TimerChannelInput : public TimerChannel
{
public:
	TimerChannelInput(TIM_TypeDef* timx, uint8_t channel)
		: TimerChannel(timx, channel)
	{
		TIM_ICStructInit(&_config);
	};

	void setEnabled(FunctionalState enabled);
	void setICPolarity(uint16_t polarity);
	void setICSelection(uint16_t selection);
	void setICPrescaler(uint16_t prescaler);
	void setICFilter(uint16_t filter);

	void _init(void);

	FunctionalState _enabled; // TODO: extend TIM_ICInitTypeDef to support this too
	TIM_ICInitTypeDef _config;
};

void TimerChannelInput::setICSelection(uint16_t selection)
{
	_config.TIM_ICSelection = selection;
	_init();
}

void TimerChannelInput::setICPolarity(uint16_t polarity)
{
	_config.TIM_ICPolarity = polarity;
	_init();
}

void TimerChannelInput::setICPrescaler(uint16_t prescaler)
{
	_config.TIM_ICPrescaler = prescaler;
	_init();
}

void TimerChannelInput::setICFilter(uint16_t filter)
{
	_config.TIM_ICFilter = filter;
	_init();
}

void TimerChannelInput::setEnabled(FunctionalState enabled)
{
	_enabled = enabled;
	TIM_CCxCmd(_peripheral, _channel, _enabled);
}

void TimerChannelInput::_init(void)
{
	_enabled = ENABLE;
	_config.TIM_Channel = _channel;
	TIM_ICInit(_peripheral, &_config); // Note: enables CCR too
}

class TimerChannelOutput : public TimerChannel
{
public:
	TimerChannelOutput(TIM_TypeDef* timx, uint8_t channel)
		: TimerChannel(timx, channel)
	{
		TIM_OCStructInit(&_config);
	};

	void setMode(uint16_t mode);

	void enableInterrupt(void);
	void disableInterrupt(void);

	void setDeadtime(uint16_t deadTime);
	void setDutyCycle(float duty);
	void setCompare(uint16_t ccr);

	void preloadConfig(FunctionalState enabled);
	void setEnabled(FunctionalState enabled);
	void setEnabledN(FunctionalState enabled);

	void setPolarity(FunctionalState high);
	void setPolarityN(FunctionalState high);

	void setIdleState(FunctionalState high);
	void setIdleStateN(FunctionalState high);

	void _init(void);

	TIM_OCInitTypeDef _config;
};

void TimerChannelOutput::setMode(uint16_t mode)
{
	_config.TIM_OCMode = mode;
	TIM_SelectOCxM(_peripheral, _channel, mode);
}

void TimerChannelOutput::setDutyCycle(float duty)
{
	uint16_t compare = map(duty, 0, 1.0f, 0, _peripheral->ARR);
	setCompare(compare);
}

void TimerChannelOutput::setCompare(uint16_t compare)
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
void TimerChannelOutput::setEnabled(FunctionalState enabled)
{
	_config.TIM_OutputState = enabled ? TIM_OutputState_Enable : TIM_OutputState_Disable;
	_init();
}

void TimerChannelOutput::setEnabledN(FunctionalState enabled)
{
    _config.TIM_OutputNState = enabled ? TIM_OutputNState_Disable : TIM_OutputNState_Disable;
    _init();
}

void TimerChannelOutput::preloadConfig(FunctionalState enabled)
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

void TimerChannelOutput::_init(void)
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

    void setMOE(FunctionalState enabled);

    void setClockEnabled(FunctionalState enabled);
    void setEnabled(FunctionalState enabled);
    void preloadConfig(FunctionalState enabled);
    void setAutoreload(uint32_t arr);
    uint32_t getAutoreload(void);
    void setFrequency(uint16_t Hz);
	void setPeriod(uint32_t microseconds);
    uint16_t getPeriod(void);

	void setPrescaler(uint16_t prescaler);

    void interruptConfig(const uint8_t interrupt, const FunctionalState enabled);
    void playNote(uint16_t frequency, uint16_t duration_ms);

    void ITConfig(uint16_t its, FunctionalState enabled);

    TIM_TypeDef* peripheral(void) { return _peripheral; };

private:
    TIM_TypeDef* _peripheral;
    TIM_TimeBaseInitTypeDef _config;
};

// TODO inline
void Timer::ITConfig(uint16_t its, FunctionalState enabled)
{
	TIM_ITConfig(_peripheral, its, enabled);
}
// Input: period (ticks), tick frequency (Hz)
// - Enable the peripheral clock
// - Configure the peripheral
// Fails if: SystemCoreClock is not divisible by input frequency

void Timer::interruptConfig(const uint8_t interrupt, const FunctionalState enabled)
{
	TIM_ITConfig(_peripheral, interrupt, enabled);

}
// TODO inline
void Timer::setMOE(FunctionalState enabled)
{
	TIM_CtrlPWMOutputs(_peripheral, enabled);
}

void Timer::setAutoreload(uint32_t arr)
{
	_config.TIM_Period;
	TIM_SetAutoreload(_peripheral, arr);
}
uint32_t Timer::getAutoreload(void)
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
