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
#include "stm32f0xx_conf.h"


#include "helpers.h"
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

	TIM_ICInitTypeDef _config;
};

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
	void setDuty(uint16_t duty);
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

	TIM_OCInitTypeDef _config;
};

class Timer
{
public:
    Timer(TIM_TypeDef* timx)
        : _peripheral(timx)
    {
    	setClockEnabled(ENABLE);
    	TIM_TimeBaseStructInit(&_config);
    };
    ~Timer() {
//    		it_callback_t* cb = ccCallbacks;
//    		while (cb) {
//    			it_callback_t* tmp = cb->next;
//    			delete(cb);
//    			cb = tmp;
//    		}
//    		cb = upCallbacks;
//    		while (cb) {
//    			it_callback_t* tmp = cb->next;
//    			delete(cb);
//    			cb = tmp;
//    		}
    } // delete callbacks

    // current config
    void _init(void);

    void _irqHandler(void) {

    	// Note TIM_GetITStatus checks flag status as well as that interrupt is enabled
    	// TIM_GetFlagStatus only checks the flag status, and you can pass a bitmask

	   if (TIM_GetITStatus(_peripheral, TIM_IT_Update)) {
			it_callback_t* cb = upCallbacks;
			while (cb) {
				cb->callback();
				cb = cb->next;
			}
			_peripheral->SR = (uint16_t)~(TIM_IT_Update);
		}

		if (IS_TIM_LIST4_PERIPH(_peripheral) && TIM_GetITStatus(_peripheral, TIM_IT_CC1)) {
			it_callback_t* cb = cc1Callbacks;
			while (cb) {
				cb->callback();
				cb = cb->next;
			}
			_peripheral->SR = (uint16_t)~(TIM_IT_CC1);
		}

		if (IS_TIM_LIST6_PERIPH(_peripheral) && TIM_GetITStatus(_peripheral, TIM_IT_CC2)) {
			it_callback_t* cb = cc2Callbacks;
			while (cb) {
				cb->callback();
				cb = cb->next;
			}
			_peripheral->SR = (uint16_t)~(TIM_IT_CC2);
		}

		if (IS_TIM_LIST3_PERIPH(_peripheral) && TIM_GetITStatus(_peripheral, TIM_IT_CC3)) {
			it_callback_t* cb = cc3Callbacks;
			while (cb) {
				cb->callback();
				cb = cb->next;
			}
			_peripheral->SR = (uint16_t)~(TIM_IT_CC3);
		}

		if (IS_TIM_LIST3_PERIPH(_peripheral) && TIM_GetITStatus(_peripheral, TIM_IT_CC4)) {
			it_callback_t* cb = cc4Callbacks;
			while (cb) {
				cb->callback();
				cb = cb->next;
			}
			_peripheral->SR = (uint16_t)~(TIM_IT_CC4);
		}
    }

    // Defaults
    void init(uint16_t prescaler = 0x0000,
			uint32_t period = 0xFFFFFFFF,
    		uint16_t counterMode = TIM_CounterMode_Up,
			uint16_t clockDivision = TIM_CKD_DIV1,
			uint8_t repetitionCounter = 0x0000);

    // contorl/config
    void setEnabled(FunctionalState enabled); // enable output
    void setMOE(FunctionalState enabled);
    void setClockEnabled(FunctionalState enabled);
    void preloadConfig(FunctionalState enabled);
    void setCCPreloadControl(FunctionalState enabled);
    void setAutoreload(uint32_t arr);
    uint32_t getAutoreload(void);
    void setFrequency(uint16_t Hz);
	void setPeriod(uint32_t microseconds);
    uint16_t getPeriod(void);

    void clearFlag(uint16_t flag) { TIM_ClearFlag(_peripheral, flag); };
	void setPrescaler(uint16_t prescaler);

    TIM_TypeDef* peripheral(void) { return _peripheral; };

    // Interrupt configuration and handling
    void interruptConfig(const uint8_t interrupts, const FunctionalState newState);

	it_callback_t* upCallbacks;
	it_callback_t* cc1Callbacks;
	it_callback_t* cc2Callbacks;
	it_callback_t* cc3Callbacks;
	it_callback_t* cc4Callbacks;

	void addCallback(it_callback_t* callbacks, it_callback_t* newCallback) {
			if (!callbacks) {
				callbacks = newCallback;
			} else {
				it_callback_t* tail = callbacks;
				while (tail->next != nullptr) {
					tail = tail->next;
				}
				tail->next = newCallback;
			}
	}

	it_callback_t* setupUpCallback(void (*upCallbackFn)(void))
	{
		it_callback_t* newCb = new it_callback_t;
		newCb->callback = upCallbackFn;
		addCallback(upCallbacks, newCb);
		return newCb;
	}

	it_callback_t* setupCc1Callback(void (*upCallbackFn)(void))
	{
		if (!IS_TIM_LIST4_PERIPH(_peripheral)) {
			return nullptr;
		}
		it_callback_t* newCb = new it_callback_t;
		newCb->callback = upCallbackFn;
		addCallback(cc1Callbacks, newCb);
		return newCb;
	}

	it_callback_t* setupCc2Callback(void (*upCallbackFn)(void))
	{
		if (!IS_TIM_LIST6_PERIPH(_peripheral)) {
			return nullptr;
		}
		it_callback_t* newCb = new it_callback_t;
		newCb->callback = upCallbackFn;
		addCallback(cc2Callbacks, newCb);
		return newCb;
	}

	it_callback_t* setupCc3Callback(void (*upCallbackFn)(void))
	{
		if (!IS_TIM_LIST3_PERIPH(_peripheral)) {
			return nullptr;
		}
		it_callback_t* newCb = new it_callback_t;
		newCb->callback = upCallbackFn;
		addCallback(cc3Callbacks, newCb);
		return newCb;
	}

	it_callback_t* setupCc4Callback(void (*upCallbackFn)(void))
	{
		if (!IS_TIM_LIST3_PERIPH(_peripheral)) {
			return nullptr;
		}
		it_callback_t* newCb = new it_callback_t;
		newCb->callback = upCallbackFn;
		addCallback(cc4Callbacks, newCb);
		return newCb;
	}

private:
    TIM_TypeDef* _peripheral;
    TIM_TimeBaseInitTypeDef _config;
};

// see stm32f05x datasheet 3.14 Timers and watchdogs
#ifdef USE_TIM_1
Timer timer1  { TIM1 }; // 16 bit Advanced control
#endif
#ifdef USE_TIM_2
Timer timer2  { TIM2 }; // 32 bit General purpose
#endif
#ifdef USE_TIM_3
Timer timer3  { TIM3 }; // 16 bit Advanced control
#endif
#ifdef USE_TIM_6
Timer timer6  { TIM6 }; // 16 bit Basic
#endif
#ifdef USE_TIM_14
Timer timer14  { TIM14 }; // 16 bit Advanced control
#endif
#ifdef USE_TIM_15
Timer timer15  { TIM15 }; // 16 bit Advanced control
#endif
#ifdef USE_TIM_16
Timer timer16  { TIM16 }; // 16 bit Advanced control
#endif
#ifdef USE_TIM_17
Timer timer17  { TIM17 }; // 16 bit Advanced control
#endif


//Timer timer2   { TIM2 }; // 32 bit General purpose
//Timer timer3   { TIM3 }; // 16 bit General purpose
//Timer timer14  { TIM4 }; // 16 bit General purpose
//Timer timer15  { TIM4 }; // 16 bit General purpose
//Timer timer16  { TIM4 }; // 16 bit General purpose
//Timer timer17  { TIM4 }; // 16 bit General purpose
//Timer timer6   { TIM4 }; // 16 bit Basic - Commutation?

/// ~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@
/// Interrupts
extern "C" {

void TIM1_CC_IRQHandler(void);

void TIM2_IRQHandler(void);
}
