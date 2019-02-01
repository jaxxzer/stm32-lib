#pragma once
#if defined(STM32F0)
#include "stm32f0xx_conf.h"
#elif defined(STM32F1)
#include "stm32f10x_conf.h"
#elif defined(STM32F3)
#include "stm32f30x_conf.h"
#else
 #error
#endif

#include "helpers.h"


class Exti
{
public:
    Exti();

    void initLine(uint32_t line = 0,
        EXTITrigger_TypeDef trigger = EXTI_Trigger_Falling,
        FunctionalState state = ENABLE,
        EXTIMode_TypeDef mode = EXTI_Mode_Interrupt);

    static const uint8_t numExti = 16;
    it_callback_t* _callbacks[numExti] = { nullptr };
    it_callback_t* _globalCallbacks = { nullptr };

    it_callback_t* setupCallback(uint8_t exti, void (*newCallbackFn)(void));
    it_callback_t* setupGlobalCallback(void (*newCallbackFn)(void));

    void _irqHandler(uint8_t exti);

    void _executeCallbacks(it_callback_t* callbacks);
};

extern Exti exti;
