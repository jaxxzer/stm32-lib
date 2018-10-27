#include "timer.h"

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

///////////////////////////////////
/////////////  Input Channel //////
///////////////////////////////////
void Timer::setCCPreloadControl(FunctionalState enabled) //  AKA CCPC
{
	TIM_CCPreloadControl(_peripheral, enabled);
}


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

///////////////////////////////////
///////////// Output Channel //////
///////////////////////////////////

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

// input duty normalized to 0~u16max
// duty is applied according to current timer resolution settings
void TimerChannelOutput::setDuty(uint16_t duty)
{
	uint16_t compare = map(duty, 0, UINT16MAX, 10, _peripheral->ARR);
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


// TIMER


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
	_initTimeBase();
}
// Input: period (ticks), tick frequency (Hz)
// - Enable the peripheral clock
// - Configure the peripheral
// Fails if: SystemCoreClock is not divisible by input frequency

void Timer::interruptConfig(const uint8_t interrupts, const FunctionalState newState)
{
	TIM_ITConfig(_peripheral, interrupts, newState);
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
#ifdef TIM14
    case TIM14_BASE:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, enabled);
        break;
#endif
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


void Timer::_initTimeBase(void)
{
    TIM_TimeBaseInit(_peripheral, &_config);
}

void Timer::_configBDTR()
{
	TIM_BDTRConfig(_peripheral, &_config_bdtr);
}

void Timer::_initBDTR(void){
	TIM_BDTRStructInit(&_config_bdtr);
	_configBDTR();
}

void Timer::setPrescaler(uint16_t prescaler)
{
	_config.TIM_Prescaler = prescaler;
	TIM_PrescalerConfig(_peripheral, prescaler, TIM_PSCReloadMode_Immediate);
}

// Sets timer frequency within bounds of current clock frequency and prescaler configuration
bool Timer::setFrequency(uint16_t f) // Hz
{
	RCC_ClocksTypeDef RCC_ClocksStruct;
	RCC_GetClocksFreq(&RCC_ClocksStruct);

#if defined(STM32F0)
	uint32_t clk_f = RCC_ClocksStruct.PCLK_Frequency / _config.TIM_Prescaler;
#elif defined(STM32F1)
	uint32_t clk_f = RCC_ClocksStruct.PCLK1_Frequency / _config.TIM_Prescaler;
#elif defined(STM32F3)
	uint32_t clk_f = RCC_ClocksStruct.PCLK1_Frequency / _config.TIM_Prescaler;
#else
#error
#endif

	if (f > clk_f) {
		return false;
	} else {
		uint16_t min_f  = clk_f / UINT16MAX;
		if (f < min_f) {
			return false;
		}
	}

	setAutoreload(clk_f / f);
	return true;
}

bool Timer::setFrequencyForce(uint16_t f)
{
	
	// f = f_PCLCK / (PSC * ARR)
	// Choose ARR and PSC, maximizing ARR (resolution)
	RCC_ClocksTypeDef RCC_ClocksStruct;
	RCC_GetClocksFreq(&RCC_ClocksStruct);
	// ex 50 hz:
	// 0x
#if defined(STM32F0)
	uint32_t pclk_f = RCC_ClocksStruct.PCLK_Frequency / (_config.TIM_Prescaler + 1);
#elif defined(STM32F1)
	uint32_t pclk_f = RCC_ClocksStruct.PCLK2_Frequency / (_config.TIM_Prescaler + 1);
#elif defined(STM32F3)
	uint32_t pclk_f = RCC_ClocksStruct.PCLK2_Frequency / (_config.TIM_Prescaler + 1);
#endif

	uint32_t PSC = pclk_f / (f * 0xFFFF);

	if (pclk_f % (f * 0xFFFF)) {
		PSC++;
	}

	uint32_t ARR = pclk_f / (PSC * f);

	setPrescaler(PSC);
	setAutoreload(ARR);
	return true;
}

// period in ticks
//void Timer::setPeriod(uint32_t microseconds)
//{
////	TIM_SetAutoreload(_peripheral, _period);
//}

void Timer::_executeCallbacks(it_callback_t* callbacks)
{
	it_callback_t* cb = callbacks;
	while (cb) {
		cb->callback();
		cb = cb->next;
	}
}

void Timer::_deleteCallbacks(void)
{
	return;
}

void Timer::_irqHandler(void)
{
	// Note TIM_GetITStatus checks flag status as well as that interrupt is enabled
	// TIM_GetFlagStatus only checks the flag status, and you can pass a bitmask
   if (TIM_GetITStatus(_peripheral, TIM_IT_Update)) {
		_executeCallbacks(upCallbacks);
		_peripheral->SR &= (uint16_t)~(TIM_IT_Update);
	}

	if (IS_TIM_LIST4_PERIPH(_peripheral) && TIM_GetITStatus(_peripheral, TIM_IT_CC1)) {
		_executeCallbacks(cc1Callbacks);
		_peripheral->SR  &=  (uint16_t)~(TIM_IT_CC1);
	}

	if (IS_TIM_LIST6_PERIPH(_peripheral) && TIM_GetITStatus(_peripheral, TIM_IT_CC2)) {
		_executeCallbacks(cc2Callbacks);
		_peripheral->SR = (uint16_t)~(TIM_IT_CC2);
	}

	if (IS_TIM_LIST3_PERIPH(_peripheral) && TIM_GetITStatus(_peripheral, TIM_IT_CC3)) {
		_executeCallbacks(cc3Callbacks);
		_peripheral->SR = (uint16_t)~(TIM_IT_CC3);
	}

	if (IS_TIM_LIST3_PERIPH(_peripheral) && TIM_GetITStatus(_peripheral, TIM_IT_CC4)) {
		_executeCallbacks(cc4Callbacks);
		_peripheral->SR = (uint16_t)~(TIM_IT_CC4);
	}
}

it_callback_t* Timer::addCallback(it_callback_t** callbacks, void (*newCallbackFn)(void)) {
	it_callback_t* newCb = new it_callback_t;
	newCb->callback = newCallbackFn;
    it_callback_t* currentCb = *callbacks;
	if (!currentCb) {
		*callbacks = newCb;
	} else {
		it_callback_t* tail = *callbacks;
		while (tail->next != nullptr) {
			tail = tail->next;
		}
		tail->next = newCb;
	}
	return newCb;
}

it_callback_t* Timer::setupUpCallback(void (*upCallbackFn)(void))
{
	upCallbacks = addCallback(&upCallbacks, upCallbackFn);
	return upCallbacks;
}

it_callback_t* Timer::setupCc1Callback(void (*cc1CallbackFn)(void))
{
	if (!IS_TIM_LIST4_PERIPH(_peripheral)) {
		return nullptr;
	}
	//return nullptr;
	//return addCallback(cc2Callbacks, cc1CallbackFn);

	return addCallback(&cc1Callbacks, cc1CallbackFn);
}

it_callback_t* Timer::setupCc2Callback(void (*cc2CallbackFn)(void))
{
	if (!IS_TIM_LIST6_PERIPH(_peripheral)) {
		return nullptr;
	}
	return addCallback(&cc2Callbacks, cc2CallbackFn);
}

it_callback_t* Timer::setupCc3Callback(void (*cc3CallbackFn)(void))
{
	if (!IS_TIM_LIST3_PERIPH(_peripheral)) {
		return nullptr;
	}
	return addCallback(&cc3Callbacks, cc3CallbackFn);
}

it_callback_t* Timer::setupCc4Callback(void (*cc4CallbackFn)(void))
{
	if (!IS_TIM_LIST3_PERIPH(_peripheral)) {
		return nullptr;
	}
	return addCallback(&cc4Callbacks, cc4CallbackFn);
}


/// ~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@~~@
/// Interrupts
extern "C" {

#ifdef STM32F0
void TIM1_BRK_UP_TRG_COM_IRQHandler(void) {
#ifdef USE_TIM_1
    timer1._irqHandler();
#endif
}
#elif STM32F1
void TIM1_UP_IRQHandler(void) {
#ifdef USE_TIM_1
    timer1._irqHandler();
#endif
}
#endif

void TIM1_CC_IRQHandler(void) {
#ifdef USE_TIM_1
    timer1._irqHandler();
	//printf("\n\r%d", TIM1->SR);
//	if (TIM_GetITStatus(TIM1, TIM_IT_CC2)) {
//		TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_ForcedAction_InActive);
//		TIM1->SR = (uint16_t)~TIM_IT_CC2;
//	} else if (TIM_GetITStatus(TIM1, TIM_IT_CC3)){
//		TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_ForcedAction_InActive);
//		flip = !flip;
//		TIM1->SR = (uint16_t)~TIM_IT_CC3;
//	} else if (TIM_GetITStatus(TIM1, TIM_IT_CC4)) {
//		if (flip) {
//			TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
//		} else {
//			TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1);
//		}
//		TIM1->SR = (uint16_t)~TIM_IT_CC4;
//	}
#endif
}

void TIM2_IRQHandler(void) {
#ifdef USE_TIM_2
	timer2._irqHandler();
#endif
}

void TIM3_IRQHandler(void) {
#ifdef USE_TIM_3
	timer3._irqHandler();
#endif
}


	/// ~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!
	/// CC2: Handler for Hall sensor input capture
	/// Update:
//	void TIM3_IRQHandler(void)
//	{
//		if (TIM_GetITStatus(TIM3, TIM_IT_CC2)) {
//			b.commutate();
//			TIM3->CNT = 0;
//			b.HallHandler(TIM3->CCR2);
//			TIM_ClearFlag(TIM3, TIM_FLAG_CC2);
//		} else if (TIM_GetITStatus(TIM3, TIM_IT_Update)) {
//			TIM_ClearFlag(TIM3, TIM_FLAG_Update);
//		}
//	}
//
//	/// ~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!
//	/// CC2: left phase for audio state
//	/// CC3: right phase for audio state
//	/// CC4: offset timing counter for audio state
//	/// Update:
//
//
//	void TIM6_DAC_IRQHandler(void)
//	{
//		//b.commutate();
////		TIM_ClearFlag(TIM6, TIM_FLAG_Update);
//	}
}
