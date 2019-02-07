#include "exti.h"
Exti::Exti() {
#if defined(STM32F0)
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
#elif defined(STM32F1)
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
#else
#warning "no support for exti"
#endif
}
void Exti::initLine(uint32_t line, EXTITrigger_TypeDef trigger, FunctionalState state, EXTIMode_TypeDef mode) {

  EXTI_InitTypeDef extiInit;
  extiInit.EXTI_Line = 1 << line;
  extiInit.EXTI_Mode = mode;
  extiInit.EXTI_Trigger = trigger;
  extiInit.EXTI_LineCmd = state;
  EXTI_Init(&extiInit);

#if defined(STM32F0)
  switch (line) {
  case 0:
  case 1:
    nvic_config(EXTI0_1_IRQn, 0, ENABLE);
    break;
  case 2:
  case 3:
    nvic_config(EXTI2_3_IRQn, 0, ENABLE);
    break;
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
  case 13:
  case 14:
  case 15:
    nvic_config(EXTI4_15_IRQn, 0, ENABLE);
    break;
  default:
    break;
  }
#elif defined(STM32F1)
  switch (line) {
  case 0:
    nvic_config(EXTI0_IRQn, 0, 0, ENABLE);
    break;
  case 1:
    nvic_config(EXTI1_IRQn, 0, 0, ENABLE);
    break;
  case 2:
    nvic_config(EXTI2_IRQn, 0, 0, ENABLE);
    break;
  case 3:
    nvic_config(EXTI3_IRQn, 0, 0, ENABLE);
    break;
  case 4:
    nvic_config(EXTI4_IRQn, 0, 0, ENABLE);
    break;
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
    nvic_config(EXTI9_5_IRQn, 0, 0, ENABLE);
    break;
  case 10:
  case 11:
  case 12:
  case 13:
  case 14:
  case 15:
    nvic_config(EXTI15_10_IRQn, 0, 0, ENABLE);
    break;
  default:
    break;
  }
#else
#warning "no support for exti"
#endif
}

it_callback_t *Exti::setupCallback(uint8_t exti, void (*newCallbackFn)(void)) {

  it_callback_t **callbacks = &_callbacks[exti];
  it_callback_t *newCb = new it_callback_t;
  newCb->callback = newCallbackFn;
  it_callback_t *currentCb = *callbacks;
  if (!currentCb) {
    *callbacks = newCb;
  } else {
    it_callback_t *tail = *callbacks;
    while (tail->next != nullptr) {
      tail = tail->next;
    }
    tail->next = newCb;
  }
  return newCb;
}
it_callback_t *Exti::setupGlobalCallback(void (*newCallbackFn)(void)) {

  it_callback_t **callbacks = &_globalCallbacks;
  it_callback_t *newCb = new it_callback_t;
  newCb->callback = newCallbackFn;
  it_callback_t *currentCb = *callbacks;
  if (!currentCb) {
    *callbacks = newCb;
  } else {
    it_callback_t *tail = *callbacks;
    while (tail->next != nullptr) {
      tail = tail->next;
    }
    tail->next = newCb;
  }
  return newCb;
}
void Exti::_irqHandler(uint8_t exti) {
  if (exti >= numExti) {
    return;
  }
  printf("%d\r\n", exti);
  _executeCallbacks(_globalCallbacks);
  _executeCallbacks(_callbacks[exti]);
  EXTI_ClearFlag(1 << exti);
}

void Exti::_executeCallbacks(it_callback_t *callbacks) {
  it_callback_t *cb = callbacks;
  while (cb) {
    cb->callback();
    cb = cb->next;
  }
}

Exti exti;

#ifdef USE_EXTI
extern "C" {

void EXTI0_IRQHandler(void) { exti._irqHandler(0); }

void EXTI1_IRQHandler(void) { exti._irqHandler(1); }

void EXTI2_IRQHandler(void) { exti._irqHandler(2); }

void EXTI3_IRQHandler(void) { exti._irqHandler(3); }

void EXTI4_IRQHandler(void) { exti._irqHandler(4); }

void EXTI9_5_IRQHandler(void) {
  for (uint8_t i = 5; i <= 9; i++) {
    if (EXTI_GetFlagStatus(1 << i)) {
      exti._irqHandler(i);
    }
  }
}

void EXTI15_10_IRQHandler(void) {
  for (uint8_t i = 10; i <= 15; i++) {
    if (EXTI_GetFlagStatus(1 << i)) {
      exti._irqHandler(i);
    }
  }
}
}
#endif
