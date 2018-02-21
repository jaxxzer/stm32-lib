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
#ifndef TIMER_H_
#define TIMER_H_

class TimerChannel
{

};

class Timer
{
public:
    Timer(TIM_TypeDef* timx)
        : _peripheral(timx)
        , _tickFrequency(1000000) // 1MHz
    {};

    void init(uint16_t period, uint32_t tickFrequency);
    void outputChannelInitPwm(uint8_t channel, uint16_t dutyCycle);
    void outputChannelInitPwmComplimentary(uint8_t channel, uint16_t dutyCycle);
    void setDutyCycle(uint8_t channel, uint16_t dutyCycle);
    void enableInterrupt(uint16_t interrupt, uint8_t priority);
    void setPrescaler(uint16_t prescaler);
    void setFrequency(uint16_t f);
    void setPeriod(uint16_t period);
    uint16_t getPeriod(void);

    TIM_TypeDef* peripheral(void) { return _peripheral; };

private:
    TIM_TypeDef* _peripheral;
    uint16_t _period; // aka ARR, number of counts before overflow
    uint32_t _tickFrequency; // used for prescaler

};

// Input: period (ticks), tick frequency (Hz)
// - Enable the peripheral clock
// - Configure the peripheral
// Fails if: SystemCoreClock is not divisible by input frequency

uint16_t Timer::getPeriod(void)
{
	return _period;
}
void Timer::init(uint16_t period, uint32_t tickFrequency)
{
	_period = period;
	_tickFrequency = tickFrequency;
    switch((uint32_t)_peripheral)
    {
    case TIM1_BASE:
    	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
        break;
    case TIM2_BASE:
    	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        break;
    case TIM3_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
        break;
    default:
        break;
    }

    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 0; // Everything is in microseconds
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = _period; // AKA ARR, period in ticks
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(_peripheral, &timerInitStructure);
    TIM_Cmd(_peripheral, ENABLE);
}

void Timer::outputChannelInitPwm(uint8_t channel, uint16_t dutyCycle)
{
    TIM_OCInitTypeDef outputChannelInit;
    outputChannelInit.TIM_OCMode = TIM_OCMode_PWM2;
    outputChannelInit.TIM_Pulse = dutyCycle; // AKA CCR1 ~ 100us pulse
    outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
    outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;

    switch (channel) {
    case TIM_Channel_1:
        TIM_OC1Init(_peripheral, &outputChannelInit);
        TIM_OC1PreloadConfig(_peripheral, TIM_OCPreload_Enable);
    	break;
    case TIM_Channel_2:
        TIM_OC2Init(_peripheral, &outputChannelInit);
        TIM_OC2PreloadConfig(_peripheral, TIM_OCPreload_Enable);
    	break;
    case TIM_Channel_3:
        TIM_OC3Init(_peripheral, &outputChannelInit);
        TIM_OC3PreloadConfig(_peripheral, TIM_OCPreload_Enable);
    	break;
    case TIM_Channel_4:
        TIM_OC4Init(_peripheral, &outputChannelInit);
        TIM_OC4PreloadConfig(_peripheral, TIM_OCPreload_Enable);
    	break;
    default:
    	break;
    }
}

void Timer::outputChannelInitPwmComplimentary(uint8_t channel, uint16_t dutyCycle)
{
    TIM_OCInitTypeDef outputChannelInit;
    outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1;
    outputChannelInit.TIM_Pulse = dutyCycle; // AKA CCR1 ~ 100us pulse
    outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
    outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;
    outputChannelInit.TIM_OutputNState = TIM_OutputNState_Enable;
    outputChannelInit.TIM_OCNPolarity = TIM_OCNPolarity_High;
    outputChannelInit.TIM_OCIdleState = TIM_OCIdleState_Reset;
    outputChannelInit.TIM_OCNIdleState = TIM_OCNIdleState_Reset; // State when Main output enable (MOE) = 0


    switch (channel) {
    case TIM_Channel_1:
        TIM_OC1Init(_peripheral, &outputChannelInit);
        TIM_OC1PreloadConfig(_peripheral, TIM_OCPreload_Disable);
    	break;
    case TIM_Channel_2:
        TIM_OC2Init(_peripheral, &outputChannelInit);
        TIM_OC2PreloadConfig(_peripheral, TIM_OCPreload_Disable);
    	break;
    case TIM_Channel_3:
        TIM_OC3Init(_peripheral, &outputChannelInit);
        TIM_OC3PreloadConfig(_peripheral, TIM_OCPreload_Disable);
    	break;
    case TIM_Channel_4:
        TIM_OC4Init(_peripheral, &outputChannelInit);
        TIM_OC4PreloadConfig(_peripheral, TIM_OCPreload_Disable);
    	break;
    default:
    	break;
    }


}

void Timer::setPrescaler(uint16_t prescaler)
{
	TIM_PrescalerConfig(_peripheral, prescaler, TIM_PSCReloadMode_Immediate);
}

void Timer::setFrequency(uint16_t f) // Hz
{
//	float period = 1.0f/f;
//	float tickPeriod = 1.0f/_tickFrequency;
//	uint16_t ticks = period/tickPeriod;
	uint16_t ticks = _tickFrequency / f;
	setPeriod(ticks);
}

// period in ticks
void Timer::setPeriod(uint16_t period)
{
	_period = period;
	TIM_SetAutoreload(_peripheral, _period);
}

void Timer::setDutyCycle(uint8_t channel, uint16_t dutyCycle)
{
	switch (channel) {
	case TIM_Channel_1:
		TIM_SetCompare1(_peripheral, dutyCycle);
		break;
	case TIM_Channel_2:
		TIM_SetCompare2(_peripheral, dutyCycle);
		break;
	case TIM_Channel_3:
		TIM_SetCompare3(_peripheral, dutyCycle);
		break;
	case TIM_Channel_4:
		TIM_SetCompare4(_peripheral, dutyCycle);
		break;
	}
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

#endif /* TIMER_H_ */
