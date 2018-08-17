#include "timer.h"

///////////////////////////////////
/////////////  Input Channel //////
///////////////////////////////////
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



