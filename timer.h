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

	// Defaults
	void init(uint16_t polarity = TIM_ICPolarity_Rising,
			uint16_t filter = 0x00,
			uint16_t prescaler = TIM_ICPSC_DIV1,
			uint16_t selection = TIM_ICSelection_DirectTI);

	// Current config
	void _init(void);

	FunctionalState _enabled; // TODO: extend TIM_ICInitTypeDef to support this too

//	typedef struct
//	{
//
//	  uint16_t TIM_Channel;      /*!< Specifies the TIM channel.
//	                                  This parameter can be a value of @ref TIM_Channel */
//
//	  uint16_t TIM_ICPolarity;   /*!< Specifies the active edge of the input signal.
//	                                  This parameter can be a value of @ref TIM_Input_Capture_Polarity */
//
//	  uint16_t TIM_ICSelection;  /*!< Specifies the input.
//	                                  This parameter can be a value of @ref TIM_Input_Capture_Selection */
//
//	  uint16_t TIM_ICPrescaler;  /*!< Specifies the Input Capture Prescaler.
//	                                  This parameter can be a value of @ref TIM_Input_Capture_Prescaler */
//
//	  uint16_t TIM_ICFilter;     /*!< Specifies the input capture filter.
//	                                  This parameter can be a number between 0x0 and 0xF */
//	} TIM_ICInitTypeDef;
	TIM_ICInitTypeDef _config;
};

void TimerChannelInput::init(uint16_t polarity,
			uint16_t filter,
			uint16_t prescaler,
			uint16_t selection)
{
	_config.TIM_ICPolarity = polarity;
	_config.TIM_ICFilter = filter;
	_config.TIM_ICPrescaler = prescaler;
	_config.TIM_ICSelection = selection;
	_init();
}

void TimerChannelInput::setICSelection(uint16_t selection)
{
	_config.TIM_ICSelection = selection;
	// TODO commit
}

void TimerChannelInput::setICPolarity(uint16_t polarity)
{
	_config.TIM_ICPolarity = polarity;
	// TODO commit
}

void TimerChannelInput::setICPrescaler(uint16_t prescaler)
{
	_config.TIM_ICPrescaler = prescaler;
	// TODO commit
}

void TimerChannelInput::setICFilter(uint16_t filter)
{
	_config.TIM_ICFilter = filter;
	// TODO commit
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

	// Current config
	void init(uint16_t ocMode = TIM_OCMode_Timing
			, uint32_t pulse = 0x0000000 // aka CCR
			, uint16_t outputState = TIM_OutputState_Disable
			, uint16_t outputNState = TIM_OutputNState_Disable
			, uint16_t ocpolarity = TIM_OCPolarity_High
			, uint16_t ocNpolarity = TIM_OCNPolarity_High
			, uint16_t ocIdleState = TIM_OCIdleState_Reset
			, uint16_t ocNIdleState = TIM_OCNIdleState_Reset);
	void _init(void);


//	typedef struct
//	{
//	  uint16_t TIM_OCMode;        /*!< Specifies the TIM mode.
//	                                   This parameter can be a value of @ref TIM_Output_Compare_and_PWM_modes */
//
//	  uint16_t TIM_OutputState;   /*!< Specifies the TIM Output Compare state.
//	                                   This parameter can be a value of @ref TIM_Output_Compare_state */
//
//	  uint16_t TIM_OutputNState;  /*!< Specifies the TIM complementary Output Compare state.
//	                                   This parameter can be a value of @ref TIM_Output_Compare_N_state
//	                                   @note This parameter is valid only for TIM1. */
//
//	  uint32_t TIM_Pulse;         /*!< Specifies the pulse value to be loaded into the Capture Compare Register.
//	                                   This parameter can be a number between 0x0000 and 0xFFFF ( or 0xFFFFFFFF
//	                                   for TIM2) */
//
//	  uint16_t TIM_OCPolarity;    /*!< Specifies the output polarity.
//	                                   This parameter can be a value of @ref TIM_Output_Compare_Polarity */
//
//	  uint16_t TIM_OCNPolarity;   /*!< Specifies the complementary output polarity.
//	                                   This parameter can be a value of @ref TIM_Output_Compare_N_Polarity
//	                                   @note This parameter is valid only for TIM1. */
//
//	  uint16_t TIM_OCIdleState;   /*!< Specifies the TIM Output Compare pin state during Idle state.
//	                                   This parameter can be a value of @ref TIM_Output_Compare_Idle_State
//	                                   @note This parameter is valid only for TIM1. */
//
//	  uint16_t TIM_OCNIdleState;  /*!< Specifies the TIM Output Compare pin state during Idle state.
//	                                   This parameter can be a value of @ref TIM_Output_Compare_N_Idle_State
//	                                   @note This parameter is valid only for TIM1. */
//	} TIM_OCInitTypeDef;
	TIM_OCInitTypeDef _config;
};

void TimerChannelOutput::init(uint16_t ocMode
							, uint32_t pulse
							, uint16_t outputState
							, uint16_t outputNState
							, uint16_t ocPolarity
							, uint16_t ocNPolarity
							, uint16_t ocIdleState
							, uint16_t ocNIdleState)
{
	_config.TIM_OCMode = ocMode;
	_config.TIM_OutputState = outputState;
	_config.TIM_OutputNState = outputNState;
	_config.TIM_Pulse = pulse;
	_config.TIM_OCPolarity = ocPolarity;
	_config.TIM_OCNPolarity = ocNPolarity;
	_config.TIM_OCIdleState = ocIdleState;
	_config.TIM_OCNIdleState = ocNIdleState;
	_init();
}

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
    	setClockEnabled(ENABLE);
    	TIM_TimeBaseStructInit(&_config);
    };

    // current config
    void _init(void);

    // Defaults
    void init(uint16_t prescaler = 0x0000,
			uint32_t period = 0xFFFFFFFF,
    		uint16_t counterMode = TIM_CounterMode_Up,
			uint16_t clockDivision = TIM_CKD_DIV1,
			uint8_t repetitionCounter = 0x0000);

    void setMOE(FunctionalState enabled);

    void setClockEnabled(FunctionalState enabled);
    void setEnabled(FunctionalState enabled);
    void preloadConfig(FunctionalState enabled);
    void setCCPreloadControl(FunctionalState enabled);
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

//    typedef struct
//    {
//      uint16_t TIM_Prescaler;         /*!< Specifies the prescaler value used to divide the TIM clock.
//                                           This parameter can be a number between 0x0000 and 0xFFFF */
//
//      uint16_t TIM_CounterMode;       /*!< Specifies the counter mode.
//                                           This parameter can be a value of @ref TIM_Counter_Mode */
//
//      uint32_t TIM_Period;            /*!< Specifies the period value to be loaded into the active
//                                           Auto-Reload Register at the next update event.
//                                           This parameter must be a number between 0x0000 and 0xFFFF.  */
//
//      uint16_t TIM_ClockDivision;     /*!< Specifies the clock division.
//                                          This parameter can be a value of @ref TIM_Clock_Division_CKD */
//
//      uint8_t TIM_RepetitionCounter;  /*!< Specifies the repetition counter value. Each time the RCR downcounter
//                                           reaches zero, an update event is generated and counting restarts
//                                           from the RCR value (N).
//                                           This means in PWM mode that (N+1) corresponds to:
//                                              - the number of PWM periods in edge-aligned mode
//                                              - the number of half PWM period in center-aligned mode
//                                           This parameter must be a number between 0x00 and 0xFF.
//                                           @note This parameter is valid only for TIM1. */
//    } TIM_TimeBaseInitTypeDef;
    TIM_TimeBaseInitTypeDef _config;
};

void Timer::setCCPreloadControl(FunctionalState enabled) //  AKA CCPC
{
	TIM_CCPreloadControl(_peripheral, enabled);
}
// TODO inline
void Timer::ITConfig(uint16_t its, FunctionalState enabled)
{
	TIM_ITConfig(_peripheral, its, enabled);
}

void Timer::init(uint16_t prescaler,
			uint32_t period,
    		uint16_t counterMode,
			uint16_t clockDivision,
			uint8_t repetitionCounter)
{
	_config.TIM_ClockDivision = clockDivision;
	_config.TIM_Period = period;
	_config.TIM_Prescaler = prescaler;
	_config.TIM_RepetitionCounter = repetitionCounter;
	_config.TIM_CounterMode = counterMode;
	_init();
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
	_config.TIM_Period = arr;
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

void Timer::_init(void)
{
    TIM_TimeBaseInit(_peripheral, &_config);
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
//	uint16_t ticks = period/tickPeriod; = tickFrequency/f;
	RCC_ClocksTypeDef RCC_ClocksStruct;
	RCC_GetClocksFreq(&RCC_ClocksStruct);

	float tickPeriod = RCC_ClocksStruct.PCLK_Frequency;
	uint16_t ticks = RCC_ClocksStruct.PCLK_Frequency / f;

	setAutoreload(ticks);
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
