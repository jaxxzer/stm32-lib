#include "gpio.h"

#if defined(STM32F0) || defined(STM32F3)
void Gpio::init(GPIOMode_TypeDef mode // GPIO_Mode_IN, GPIO_Mode_OUT, GPIO_Mode_AF, GPIO_Mode_AN
                ,
                GPIOPuPd_TypeDef pupd, GPIOOType_TypeDef otype, GPIOSpeed_TypeDef speed) {
  GPIO_InitTypeDef _config;
  _config.GPIO_Mode = mode;
  _config.GPIO_PuPd = pupd;
  _config.GPIO_OType = otype;
  _config.GPIO_Speed = speed;
  _config.GPIO_Pin = (_pin);
  GPIO_Init(_port, &_config);
  reset();
}
#elif defined(STM32F1)
void Gpio::init(GPIOMode_TypeDef mode // GPIO_Mode_IN, GPIO_Mode_OUT, GPIO_Mode_AF, GPIO_Mode_AN
                ,
                GPIOSpeed_TypeDef speed) {
  GPIO_InitTypeDef _config;
  _config.GPIO_Mode = mode;
  _config.GPIO_Speed = speed;
  _config.GPIO_Pin = (_pin);
  GPIO_Init(_port, &_config);
  reset();
}
#else
#error
#endif
void Gpio::addExtiCallback(void (*newCallbackFn)(void)) { exti.setupCallback(_pinSource, newCallbackFn); }

void Gpio::extiConfig(FunctionalState state, EXTITrigger_TypeDef trigger) {
  exti.initLine(_pinSource, trigger, state);

#if defined(STM32F0)
  switch ((uint32_t)_port) {

  case GPIOA_BASE:
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, _pinSource);
    break;
  case GPIOB_BASE:
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, _pinSource);
    break;
  case GPIOC_BASE:
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, _pinSource);
    break;
  case GPIOD_BASE:
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, _pinSource);
    break;
  default:
    break;
  }
#elif defined(STM32F1)
  switch ((uint32_t)_port) {

  case GPIOA_BASE:
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, _pinSource);
    break;
  case GPIOB_BASE:
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, _pinSource);
    break;
  case GPIOC_BASE:
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, _pinSource);
    break;
  case GPIOD_BASE:
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, _pinSource);
    break;
  default:
    break;
  }

#else
#error
#endif
}
Gpio::Gpio(GPIO_TypeDef *port, uint16_t pin) : _port(port), _pin(1 << pin), _pinSource(pin) {
  _clockEnable();
  // GPIO_StructInit(&_config);
}

void Gpio::_clockEnable(void) {
  switch ((uint32_t)_port) {
#if defined(STM32F0) || defined(STM32F3)
  case GPIOA_BASE:
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    break;
  case GPIOB_BASE:
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    break;
  case GPIOC_BASE:
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    break;
  case GPIOD_BASE:
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    break;
#elif defined(STM32F1)
  case GPIOA_BASE:
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    break;
  case GPIOB_BASE:
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    break;
  case GPIOC_BASE:
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    break;
  case GPIOD_BASE:
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    break;
#else
#error
#endif

  default:
    break; // failure
  }
}

// void Gpio::init(void)
//{
//	_config.GPIO_Pin = (_pin);
//	GPIO_Init(_port, &_config);
//}

// void Gpio::gpioInit(GPIO_InitTypeDef* gpioInitStruct) {
//	_config = *gpioInitStruct;
//	init(); // TODO warn if pin mismatch
//}

// alternate function config
// See datasheet section 4 'Pinouts and pin descriptions'
#if defined(STM32F0) || defined(STM32F3)
void Gpio::configAF(uint8_t af) { GPIO_PinAFConfig(_port, _pinSource, af); }
#elif defined(STM32F1)
void Gpio::remapConfig(uint32_t remap, FunctionalState newstate) {
  // All valid remaps are non-zero
  // Zero here indicates no remap
  if (remap) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(remap, newstate);
  }
}
#else
#error
#endif

void Gpio::set(bool set) {
  bool state = _inverted ? !set : set;

  if (state) {
    // GPIO_SetBits(_port, _pin);
    _port->BSRR = _pin;
  } else {
    _port->BRR = _pin;
  }
}

void Gpio::reset(void) {
  // GPIO_ResetBits(_port, _pin);
  set(false);
}

bool Gpio::readOutput(void) {
  // return GPIO_ReadOutputDataBit(_port, _pin);
  bool state = _port->ODR & _pin;
  return _inverted ? !state : state;
}

bool Gpio::readInput(void) {
  // return GPIO_ReadInputDataBit(_port, _pin);
  bool state = _port->IDR & _pin;
  return _inverted ? !state : state;
}

void Gpio::toggle(void) {
  BitAction new_state = (BitAction)!readOutput();
  // GPIO_WriteBit(_port, _pin, new_state);

  if (new_state != Bit_RESET) {
    set(true);
  } else {
    set(false);
  }
}
